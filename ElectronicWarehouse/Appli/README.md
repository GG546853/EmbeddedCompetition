# STM32N647 AI 推理调试记录

## 项目背景

- **平台**: STM32N647X0HXQ (Cortex-M55 + Neural-ART NPU)
- **工具链**: STM32CubeIDE + GCC ARM Toolchain
- **RTOS**: FreeRTOS (CMSIS-RTOS v2)
- **AI SDK**: STEdgeAI 4.0 (`C:\ST\STEdgeAI\4.0\`)
- **模型**: BlazeFace 人脸检测 (`Model/network.c`)
- **OSAL**: `LL_ATON_OSAL_FREERTOS` (FreeRTOS 信号量同步)
- **RT 模式**: `LL_ATON_RT_ASYNC` (中断驱动 Epoch Block 执行)

---

## 调试时间线

### Phase 1: 编译错误修复

**现象**: `HAL_CACHEAXI_Init`、`HAL_CACHEAXI_Enable`、`HAL_CACHEAXI_CleanInvalidByAddr` 等函数未定义引用，`CACHEAXI_HandleTypeDef` 类型未知。

**根因**: STM32CubeIDE 在重新生成项目时自动将 `stm32n6xx_hal_conf.h` 中的 `#define HAL_CACHEAXI_MODULE_ENABLED` 注释掉，导致 `stm32n6xx_hal_cacheaxi.h` 和相关代码被排除编译。

**解决**:
1. 在 `Core/Inc/stm32n6xx_hal_conf.h` 中去掉 `HAL_CACHEAXI_MODULE_ENABLED` 宏的注释
2. 确保 `Drivers/STM32N6xx_HAL_Driver/Inc/stm32n6xx_hal_cacheaxi.h` 和 `Src/stm32n6xx_hal_cacheaxi.c` 存在
3. 在 `Debug/Drivers/STM32N6xx_HAL_Driver/subdir.mk` 中手动添加 `stm32n6xx_hal_cacheaxi.c` 的构建条目

---

### Phase 2: `stai_network_init()` 初始化失败

**现象**: `stai_network_init(network_context)` 返回 `STAI_ERROR_NETWORK_INVALID_CONTEXT_HANDLE` (0x030000)。

**根因**: `network_context_buf` 放置在 `.EXTRAM` (HyperRAM, 0x90000000) 段中。链接脚本将该段标记为 `NOLOAD`，启动代码不会对其进行零初始化。HyperRAM 上电后包含随机值，`stai_network_init()` 检查 `ctx->network_instance.network != NULL` 时读到非零值，误判为"已初始化"而返回错误。

**解决**: 在 `AI_task.c` 的 `NN_Init()` 中，调用 `stai_network_init()` 之前添加:
```c
memset(network_context_buf, 0, STAI_NETWORK_CONTEXT_SIZE);
```

**相关文件**: `APP/AI_task.c:193`

---

### Phase 3: NPU Epoch Controller AXI 总线错误

**现象**: 程序进入 `ATON_STD_IRQHandler` 后触发 `__LL_ATON_RT_IrqErr`，打印:
```
Epoch Controller ERROR interrupt: EC_IRQ = 0x00000008
Epoch Controller opcode counter: 0x00000000
Epoch Controller label: 0x00000000
```
随后 `LL_ATON_ASSERT(false)` 触发死循环。

**根因**: NPU (CID_1) 通过 AXI 访问 octoFlash (0x70000000+) 和 AXISRAM (0x34200000+) 时，RIF (Resource Isolation Framework) / RISAF (Resource Isolation Slave Address Filter) 没有为 CID_1 配置内存访问权限，导致 AXI 总线返回错误响应。

**解决**: 在 `RI_Init()` 或相关初始化代码中配置 RISAF，为 NPU Master (CID_1, SEC|PRIV) 授予以下区域访问权限:
- octoFlash (XSPI2): 0x70000000 - 0x70FFFFFF (至少读权限，存放模型权重)
- AXISRAM3-6: 0x34200000 - 0x343AFFFF (读写权限，NPU 专用 SRAM)

**相关文件**: `APP/AI_task.c:143-176` (`NPU_Init()` 中的 RIMC 配置)

---

### Phase 4: BARE_METAL OSAL 下的 WFE/SysTick 问题

**现象**: 修复 RISAF 后，程序不再触发 assertion，但 `stai_network_run()` 一直在 `__ll_aton_stai_run_synchonously()` 内部循环，无法完成推理。

**根因**: 使用 `LL_ATON_OSAL_BARE_METAL` 时，`LL_ATON_OSAL_WFE()` = `__WFE()`，这会因 **任何中断**（包括 FreeRTOS 的 1kHz SysTick）而唤醒。`__WFE()` 被 SysTick 提前唤醒后，`triggered_events` 尚未被 NPU ISR 设置，`LL_ATON_RT_RunEpochBlock()` 检查不通过，再次返回 `LL_ATON_RT_WFE`，形成无效空转循环。

**解决**: 将 OSAL 切换为 FreeRTOS 模式:
- 编译器宏: `LL_ATON_OSAL=LL_ATON_OSAL_FREERTOS`
- `LL_ATON_OSAL_WFE()` → `xSemaphoreTake(_wfe_sem, portMAX_DELAY)` (信号量阻塞)
- `LL_ATON_OSAL_SIGNAL_EVENT()` → `xSemaphoreGiveFromISR(_wfe_sem, ...)` (ISR 中释放信号量)

**相关文件**: `.cproject` (编译器定义), `ll_aton_osal_freertos.h`, `ll_aton_osal_rtos_template.c`

---

### Phase 5: FREERTOS OSAL 下 NPU ISR 不触发

**现象**: 切换到 FreeRTOS OSAL 后，`ATON_STD_IRQHandler` 断点永远不进。任务阻塞在 `xSemaphoreTake(_wfe_sem, portMAX_DELAY)`，其他 FreeRTOS 任务正常。

#### 5.1 中断路由排查（已排除的问题）

| 检查项 | 方法 | 结果 | 结论 |
|---|---|---|---|
| ISR 向量表 | `arm-none-eabi-nm` | `NPU0_IRQHandler` → `ATON_STD_IRQHandler` | **正确链接** |
| NVIC 使能 | `*(uint32_t*)(0xE000E104) >> 21 & 1` | = 1 | **CDNN0_IRQn 已使能** |
| PRIMASK | 调试器读寄存器 | = 0 | **无全局中断屏蔽** |
| BASEPRI | 调试器读寄存器 | = 0 | **无 FreeRTOS BASEPRI 屏蔽** |
| NVIC ISPR 挂起 | `*(uint32_t*)(0xE000E204) >> 21 & 1` | = 0 | **NPU 未产生中断** |
| NPU 中断优先级 | `_FinalizeIRQHandling_()` 置空测试 | = 0 (默认最高) | **优先级不相关** |

结论: NVIC 配置正确，问题在 NPU 硬件层——Epoch Controller 没有产生完成中断。

#### 5.2 NPU 内部寄存器排查

| 寄存器 | 地址 | 值 | 含义 |
|---|---|---|---|
| INTCTRL INTREG | 0x580E1008 | 0x00000000 | **无待处理中断** |
| INTCTRL INTANDMSK | 0x580E1024 | 0xFFFFF000 | SE 中断 AND 使能，EC 中断 OR 通道 |
| INTCTRL INTORMSK | 0x580E1014 | 0x000003FF | SE 中断 OR 屏蔽，EC/错误 OR 使能 |
| EC CTRL | 0x580FE000 | 0x80000001 | **RUNNING=1 (bit 31)**, EN=1 (bit 0) |
| EC IRQ | 0x580FE00C | 0x00010000 | SM 位=1, IRQ 完成位=0 → **未完成** |
| EC BC | 0x580FE020 | 148 | 执行了 148 条 opcode (一直不变) |
| EC ADDR | 0x580FE008 | 0x341B4B80 | Blob 位于内部 SRAM |
| EC LABEL | 0x580FE01C | 3 | 当前 label = 3 |

结论: **Epoch Controller 正在运行 (RUNNING=1)，但 BC 卡在 148 不再增长**。NPU 没有卡在中断路由上，而是硬件执行层面卡住了。

#### 5.3 Streaming Engine 排查

| SE | CTRL | RUNNING (bit 31) | DSTEXTADDR | 状态 |
|---|---|---|---|---|
| SE0 | 0x00000001 | 0 | - | 空闲 |
| SE1 | 0x00000001 | 0 | - | 空闲 |
| SE2 | 0x00000001 | 0 | - | 空闲 |
| SE3 | 0x00000000 | 0 | - | 禁用 |
| **SE4** | 0x80080F85 | **1** | 0x00000000 | **运行中 (卡住)** |
| **SE5** | 0x80080F01 | **1** | **0x70212160** | **运行中 (卡住)** |
| **SE6** | 0x80080E09 | **1** | 0x34304000 | **运行中 (卡住)** |
| SE7 | 0x00000000 | 0 | - | 禁用 |
| **SE8** | 0x80080F09 | **1** | 0x00000000 | **运行中 (卡住)** |
| **SE9** | 0x80080F85 | **1** | 0x34300000 | **运行中 (卡住)** |

所有卡住的 SE 共享同一个 SRCEXTADDR = 0x003E4E01:
- **SE5 DSTEXTADDR = 0x70212160** → octoFlash (XSPI2 存储空间)
- 其他 SE 目标在 AXISRAM3-6 或内部 SRAM

BUSIF ERR:
- BUSIF0 ERR (0x580EF010) = 0x10630703
- BUSIF1 ERR (0x580F0010) = 0x10630703

---

### Phase 6: 根因确认

**根因**: RISAF/RIF 内存权限配置**不完整**。

虽然之前修复了 Epoch Controller 的 AXI 总线错误 (EC_IRQ=0x08)，但当前的 RISAF 配置仍未覆盖 NPU 需要访问的**全部内存区域**。具体来说:

1. **octoFlash (0x70000000 - 0x70FFFFFF)**: SE5 的目标地址 0x70212160 在此区域。模型权重存储在此，NPU 需要通过 DMA 读取。**如果没有 RISAF 读权限，SE 的 AXI 事务会挂起 (无响应)，导致 SE 永远 RUNNING。**

2. **AXISRAM3-6 (0x34200000 - 0x343AFFFF)**: SE6/SE9 的目标地址在此区域。这些是 NPU 专用 SRAM，必须授予 CID_1 读写权限。

**关键理解**: BARE_METAL OSAL 下看似"能跑"只是假象——FreeRTOS SysTick 每 1ms 唤醒 `__WFE()`，掩盖了 NPU 硬件层面的挂起问题。切换到 FREERTOS OSAL 后，信号量不会无故释放，才暴露了真正的硬件死锁。

---

## NPU 内存访问需求汇总

| 内存区域 | 地址范围 | 用途 | NPU 需要权限 | RIF CID | RISAF 配置 |
|---|---|---|---|---|---|
| octoFlash (XSPI2) | 0x70000000 - 0x71000000 | 模型权重 (9MB) | **读** | CID_1 + SEC + PRIV | ⚠️ **待确认** |
| AXISRAM3 | 0x34200000 - 0x3426FFFF | NPU 数据/中间结果 | 读写 | CID_1 + SEC + PRIV | ⚠️ 待确认 |
| AXISRAM4 | 0x34270000 - 0x342DFFFF | NPU 数据/中间结果 | 读写 | CID_1 + SEC + PRIV | ⚠️ 待确认 |
| AXISRAM5 | 0x342E0000 - 0x3434FFFF | NPU 数据/中间结果 | 读写 | CID_1 + SEC + PRIV | ⚠️ 待确认 |
| AXISRAM6 | 0x34350000 - 0x343BFFFF | NPU 数据/中间结果 | 读写 | CID_1 + SEC + PRIV | ⚠️ 待确认 |
| 内部 SRAM | 0x34000000 - 0x341FFFFF | Epoch Blob (0x341B4B80) | 读 | CID_1 + SEC + PRIV | ⚠️ 待确认 |

---

## 关键文件

| 文件路径 | 作用 |
|---|---|
| `APP/AI_task.c` | AI 推理任务 (NPU 初始化, 推理循环) |
| `Model/stai_network.c` | ST.AI 网络封装 |
| `Model/network.c` | 模型数据 (权重, epoch blobs) |
| `.cproject` | 编译器定义 (LL_ATON_OSAL, LL_ATON_RT_MODE 等) |
| `Core/Inc/stm32n6xx_hal_conf.h` | HAL 模块使能 (HAL_CACHEAXI_MODULE_ENABLED) |
| `Core/Inc/FreeRTOSConfig.h` | FreeRTOS 配置 (优先级, TrustZone 等) |
| `STM32N647X0HXQ_LRUN_RAMxspi1.ld` | 链接脚本 (RAM, EXTRAM 定义) |
| `C:\ST\STEdgeAI\4.0\Middlewares\ST\AI\Npu\ll_aton\ll_aton_runtime.c` | NPU 运行时 (ISR, Epoch Block 执行) |
| `C:\ST\STEdgeAI\4.0\Middlewares\ST\AI\Npu\ll_aton\ll_aton_osal_freertos.h` | FreeRTOS OSAL 宏定义 |
| `C:\ST\STEdgeAI\4.0\Middlewares\ST\AI\Npu\ll_aton\ll_aton_osal_rtos_template.c` | RTOS OSAL 实现 (信号量, DAO 锁) |

---

## NPU 寄存器地址速查 (ATON_BASE = 0x580E0000)

| 寄存器 | 地址 | 说明 |
|---|---|---|
| EPOCHCTRL0 CTRL | 0x580FE000 | bit 31=RUNNING, bit 0=EN |
| EPOCHCTRL0 ADDR | 0x580FE008 | Epoch Blob 地址 |
| EPOCHCTRL0 IRQ | 0x580FE00C | bit 0=完成中断, bit 16=SM |
| EPOCHCTRL0 BC | 0x580FE020 | Opcode 计数器 |
| EPOCHCTRL0 LABEL | 0x580FE01C | 当前 Label |
| INTCTRL0 INTREG | 0x580E1008 | 中断状态寄存器 |
| INTCTRL0 INTANDMSK | 0x580E1024 | AND 掩码 (0=使能, 1=屏蔽) |
| INTCTRL0 INTORMSK | 0x580E1014 | OR 掩码 (0=使能, 1=屏蔽) |
| BUSIF0 ERR | 0x580EF010 | 总线接口错误状态 |
| BUSIF1 ERR | 0x580F0010 | 总线接口错误状态 |
| STRENGx CTRL | 0x580E2000 + x*0x1000 | bit 31=RUNNING, bit 0=EN |
| STRENGx SRCEXTADDR | 0x580E2004 + x*0x1000 | DMA 源地址 |
| STRENGx DSTEXTADDR | 0x580E2008 + x*0x1000 | DMA 目标地址 |
| NVIC ISER1 | 0xE000E104 | 中断使能 (IRQ 32-63) |
| NVIC ISPR1 | 0xE000E204 | 中断挂起 (IRQ 32-63) |

---

---

### Phase 7: RISAF 配置修复（2026-05-31）

**现象**: 经过之前的所有修复 (IAC, npu_cache, cache coherency) 后，NPU SE 仍然卡住，SRCEXTADDR=0x003E4E01，BUSIF ERR=0x10630703。

**根因**: 项目运行在 **LRUN 模式** (调试器直接加载到 RAM)，没有经过 FSBL。FSBL 在正常启动流程中会配置 RISAF (Resource Isolation Slave Address Filter) 授予 NPU (CID_1) 对 octoFlash、HyperRAM、AXISRAM 的访问权限。LRUN 模式跳过 FSBL，RISAF 处于硬件复位状态，默认拒绝 CID_1 的所有 AXI 事务，导致 NPU 的 Streaming Engines 在尝试 DMA 数据时收到总线错误并挂起。

**解决**:
1. 在 `AI_task.c` 的 `NPU_Init()` 中配置 RISAF4/5/6:
   - **RISAF4**: XSPI2 octoFlash (0x70000000-0x71FFFFFF) → CID_1 只读
   - **RISAF5**: XSPI1 HyperRAM (0x90000000-0x9FFFFFFF) → CID_1 读写
   - **RISAF6**: AXI SRAM (0x34000000-0x343FFFFF) → CID_1 读写
2. 将 NPU RIMC Master 配置从 `AI_task.c` 移至 `main.c` 的 `SystemIsolation_Config()`，与其他 Master 配置保持一致
3. 移除 `AI_task.c` 中重复的 `#include "npu_cache.h"`

**相关文件**: `APP/AI_task.c:178-202`, `Core/Src/main.c:238`

**结果**: ❌ **未解决**。RISAF 配置后 NPU SE 仍然挂起，BUSIF 错误依旧。RISAF 不是根因。

---

### Phase 8: 深入分析 — 根因重新定位（2026-05-31）

**RISAF 配置未生效的原因分析**:

1. RISAF4/5/6 作用于 4GB 地址空间，StartAddress/EndAddress 需使用**绝对物理地址**作为 offset。
2. 但 STM32N6 的一个关键细节是：RISAF 的 StartAddress/EndAddress **可能不是绝对地址，而是相对其保护地址空间基址的偏移**。例如 RISAF4 保护 XSPI2 空间，基址可能是 0x00000000 (而非 0x70000000)，此时 offset 0x70000000 会访问 XSPI2 空间的 0x70000000 位置，即绝对地址 0x70000000。但如果基址 ≠ 0，则地址映射可能不正确。
3. 实际上，从 STM32N6 内存映射看，XSPI2 的地址在 0x70000000-0x71FFFFFF，如果 RISAF4 的基址是 0x00000000，用绝对地址作为 offset 是**正确**的。
4. 但即使 RISAF 配置正确，**问题可能不在内存访问权限，而在 NPU 读取的 epoch blob 数据本身就有问题**。

**关于 SRCEXTADDR=0x003E4E01 的深入分析**:

- 0x003E4E01 处于内部 Flash 别名区 (0x00000000-0x003FFFFF)，但 STM32N647 仅有 2MB 物理 Flash (0x00000000-0x001FFFFF)
- 地址 0x003E4E01 (~4MB) 远超出物理 Flash 范围，**绝对无效**
- 该地址不可能来自正确编译的 epoch blob — blob 中所有地址均为 0x34xxxxxx 或 0x70xxxxxx 范围
- **可能原因**: NPU 从错误的地址读取了 epoch blob 内容 (垃圾数据)，将其解析为 SE 配置命令

**关键线索**:
- EC ADDR = 0x341B4B80 (epoch blob 应在的位置，位于 AXISRAM2)
- EC BC = 148 (执行了 148 条 opcode 后卡住)
- **AXISRAM2 时钟可能未使能**: 当前 `NPU_Init()` 只使能了 AXISRAM3/4/5/6，**没有使能 AXISRAM2**
- 如果 AXISRAM2 未使能，NPU 从 0x341B4B80 读取时获取全零或垃圾数据
- 还需要确认 `MX_RAMCFG_Init()` 是否已使能 AXISRAM1 和 AXISRAM2

**另一个关键差异 — API 层级**:
- 官方例程: `LL_ATON_RT_Main(&NN_Instance_Default)` — 直接低层 API
- 本项目: `stai_network_run(network_context, STAI_MODE_SYNC)` — ST.AI 高层封装 (`--generate-stai = true`)
- 高层封装内部走了 `stai_network_init()` → `__ll_aton_stai_init_network_instance()` → `LL_ATON_RT_Init_Network()` → `ec_network_init()` 路径
- `ec_network_init()` 是 stub (直接 return true)，可能有额外的初始化步骤在 ST.AI 封装中被遗漏

**模型编译参数**:
- `--enable-virtual-mem-pools = true` — 启用虚拟内存池
- `--generate-stai = true` — 生成 ST.AI 封装
- `--cache-maintenance = true` — 启用 cache 维护

其中 `--enable-virtual-mem-pools` 是关键 — 虚拟池需要在 NPU 硬件级别配置池基址表，**如果 ST.AI 封装未正确初始化此表，epoch blob 中的 pool 引用将无法解析为正确的物理地址**。

**下一步**:
1. 检查 AXISRAM2 时钟是否已使能
2. 通过调试器读取 EC blob 地址 0x341B4B80 处的前 64 字节，与 `network_ecblobs.h` 中的 `_ec_blob_network_1[]` 比对，确认 blob 数据是否正确加载
3. 考虑重新生成模型，关闭 `--generate-stai` 和 `--enable-virtual-mem-pools`，改用 `LL_ATON_RT_Main` 直接调用 (与官方例程完全一致)
4. 或在保持现有 ST.AI 封装的前提下，手动配置 NPU 虚拟内存池基址表

---

## Phase 9 — 2026-05-31 深度代码追踪与缓存/输出分析 (未解决)

### 9.1 追查过程

**彻底排查了整个 ST.AI → LL_ATON 调用链**:

1. `stai_network_run()` → `__ll_aton_stai_run()` → `__ll_aton_stai_run_synchonously()` → `LL_ATON_RT_RunEpochBlock()`
2. `stai_network_init()` → `__ll_aton_stai_init_network_instance()` → `LL_ATON_RT_Init_Network()` → `nn_instance->network->ec_network_init()`

**ec_network_init (LL_ATON_EC_Network_Init_network)** 定义在 `network_ecblobs.h:3007`:
- `memset()` 初始化 `prev_base_addrs` 数组为 0
- `ec_copy_blob()` 将编译时常量 blob 复制到运行时 BSS 缓冲区:
  - `_ec_blob_network_1` (const, 0x34034a40) → `_ec_blob_network_1_ptr` (BSS, 0x341a0600)
  - `_ec_blob_network_61` → `_ec_blob_network_61_ptr` (BSS, 0x341a1a80)
  - `_ec_blob_network_65` → `_ec_blob_network_65_ptr`

**ec_inference_init (LL_ATON_EC_Inference_Init_network)** 定义在 `network_ecblobs.h:3044`:
- NPU cache clean+invalidate 所有运行时 blob 缓冲区
- `ec_reloc()` 将当前内存池基址 patch 进 blob:
  - Input pool 12: `_mem_pool__user_io_input_0_network` → 由 `stai_network_set_inputs()` 设置为 `nn_in` (0x34053320)
  - Output pool 13-16: `_mem_pool__user_io_output_{0,1,2,3}_network` → **从未设置，均为 NULL (0x00000000)**
- MCU cache clean (SCB_CleanDCache_by_Addr) 对每个 blob 执行 → **缓存一致性已正确处理**

### 9.2 排除的假设

1. **❌ AXISRAM2 未使能**: `.bss` 段位于 AXISRAM2 (0x34178740-0x341AB780)，程序正常运行，FreeRTOS、DCMIPP、LCD 均正常 → AXISRAM2 工作正常
2. **❌ CPU 缓存一致性**: `network_ecblobs.h:3082-3086` 已调用 `LL_ATON_Cache_MCU_Clean_Range` → `mcu_cache_clean_range` → `SCB_CleanDCache_by_Addr` — CPU cache 在 `ec_reloc` 后确实被清理
3. **❌ RISAF 配置失败**: `HAL_RIF_RISAF_ConfigBaseRegion()` 未被 `CPU_AS_TRUSTED_DOMAIN` 宏包裹 → 编译通过，函数调用生效
4. **❌ MPU 配置缺失**: 官方例程也没有 MPU 配置 → 不是必要步骤
5. **❌ 魔数 0x003E4E01 在 blob 中**: 在所有 epoch blob 中均不包含该值 → 由 NPU 运行时计算得出

### 9.3 关键发现 — 输出缓冲区未设置

**用户代码只设置了输入缓冲区，从未设置输出缓冲区**:

官方例程 (991_AI_People_Detection/app.c:155-158):
```c
LL_ATON_Set_User_Input_Buffer_Default(0, capture_buffer, nn_in_len);   // 设置输入
SCB_InvalidateDCache_by_Addr(output_buffer, nn_out_len);
LL_ATON_Set_User_Output_Buffer_Default(0, output_buffer, nn_out_len); // 设置输出！
LL_ATON_RT_Main(&NN_Instance_Default);
```

用户代码 (AI_task.c:240-247):
```c
stai_ptr nn_in_ptr = nn_in;
ret = stai_network_set_inputs(network_context, &nn_in_ptr, info.n_inputs); // 只设置输入！
// 未调用 stai_network_set_outputs()
ret = stai_network_get_outputs(network_context, nn_out, &n_outputs);     // 获取输出地址
```

模型定义 (network.c:106-113):
```c
static unsigned char *_mem_pool__user_io_output_0_network = NULL; // 初始 NULL!
static unsigned char *_mem_pool__user_io_output_1_network = NULL;
static unsigned char *_mem_pool__user_io_output_2_network = NULL;
static unsigned char *_mem_pool__user_io_output_3_network = NULL;
```

**所有输出池指针初始为 NULL，且从未被赋值**。

`stai_network_get_outputs()` 返回的是 `_mem_pool__user_io_output_X_network` 的当前值 → 全部为 NULL → `nn_out[0..3]` 全部为 NULL (0x00000000)。

当 `ec_reloc()` 将这些 NULL 地址 patch 进 epoch blob 时，NPU 的 SE 尝试访问 `0x00000000 + offset` 范围内的地址。虽然 0x003E4E01 不直接是 `0x00000000 + offset` (offset 太大，~3.9MB)，但 NULL 输出池基址会导致后续 epoch 的输出/中间数据写入错误位置，连锁导致下游 epoch 从错误地址读取。

### 9.4 其他关键差异 — 用户 vs 官方

| 方面 | 官方例程 | 用户代码 |
|------|---------|---------|
| API | `LL_ATON_RT_Main()` | `stai_network_run()` |
| 模型编译 | 未使用 `--generate-stai` | `--generate-stai = true` |
| 虚拟内存池 | 未使用 | `--enable-virtual-mem-pools = true` |
| 输入缓冲位置 | `.EXTRAM` (HyperRAM) | `.noncacheable` (内部 SRAM) |
| 输入设置频率 | 每帧设置 | 初始化时一次性设置 |
| 输出缓冲 | 每帧设置 | **从未设置** |
| 双缓冲 | 有 (乒乓缓冲) | 无 |

### 9.5 虚拟内存池分析

模型使用 `--enable-virtual-mem-pools = true` 编译。内存池定义 (network.c:76-80):
```
index=11: AXISRAM2_AXISRAM3_AXISRAM4_AXISRAM5_AXISRAM6
          offset=0x34100000 absolute_mode size=2883576 vpool READ_WRITE
index=4:  AXISRAM2
          offset=0x34100000 absolute_mode size=1048576 READ_WRITE
index=5:  AXISRAM1
          offset=0x34080000 absolute_mode size=0 READ_WRITE
```

虚拟池模式下，NPU 内部有一张池基址表。如果该表未被正确初始化，NPU 会以 0 为基址计算物理地址。`ec_reloc()` 将正确的物理地址直接写入 blob 指令，但 blob 指令是否需要池基址表的配合才能正确工作尚不确定。

### 9.6 建议方案

**方案 A (推荐)**: 重新生成模型
- 关闭 `--generate-stai = false` 和 `--enable-virtual-mem-pools = false`
- 参照官方例程使用 `LL_ATON_RT_Main()` 直接调用
- 每帧设置输入和输出缓冲区
- 与官方 991_AI_People_Detection 的 pipeline 架构完全一致

**方案 B**: 修复当前代码
- 在 `NN_Init()` 中添加 `stai_network_set_outputs()` 调用，提供预分配的输出缓冲区
- 继续使用 ST.AI 高层 API
- 风险: 虚拟内存池配置可能还有更深层的问题

**方案 C**: 使用 STM32Cube AI Studio
- 最新的 STM32Cube AI Studio 提供可视化模型导入和代码生成
- 自动生成与开发板匹配的完整项目
- 这是用户 git log 中提到的方向 ("试试STM32Cube AI Studio")
