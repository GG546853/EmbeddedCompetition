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

## 下一步

1. **完整配置 RISAF**: 确保 NPU (CID_1, SEC|PRIV) 对上述所有内存区域拥有正确权限
2. **验证修复**: 重新编译烧录，观察 ISR 是否正常触发、任务是否完成推理
3. **如果 RISAF 完整配置后仍卡住**: 检查 SE 的目标地址是否落在已授权范围外，逐一排查
