优化日志
日期：2026-10-22
主题：P0-1 DWT -> FreeRTOS 运行时统计（1 秒间隔差值版）

============================================================
一、背景与目标
============================================================

1. FreeRTOSConfig.h 中已经打开运行时统计：
   - configGENERATE_RUN_TIME_STATS = 1
   - portCONFIGURE_TIMER_FOR_RUN_TIME_STATS = configureTimerForRunTimeStats
   - portGET_RUN_TIME_COUNTER_VALUE = getRunTimeCounterValue
   但 app_freertos.c 中这两个钩子是空壳，统计实际上没有工作。

2. Percepio View 快照每次约 753 个事件，只覆盖约 100ms，适合看单帧因果链
   （中断 -> 信号量 -> 任务唤醒），不适合观察分钟级的任务负载和趋势。

3. 目标：
   - 每秒输出一次各任务的 CPU 占比；
   - 同时输出每个任务的栈高水位（剩余栈空间）；
   - 不依赖调试器，可在 Release 版本长期运行；
   - 解决 32 位 DWT CYCCNT 每 7.16 秒回绕导致百分比失真的问题。

============================================================
二、改动的文件
============================================================

仅修改：
    ElectronicWarehouse/Appli/Core/Src/app_freertos.c

未修改 FreeRTOSConfig.h。
未修改 configRUN_TIME_COUNTER_TYPE（保持 size_t，即 32 位）。

============================================================
三、具体改动
============================================================

1. 增加头文件
------------------------------------------------------------

#include <stdint.h>

用于 uint32_t / uint64_t 运算。


2. 增加统计参数宏（USER CODE BEGIN PD 区域）
------------------------------------------------------------

#define STATS_MAX_TASKS       32
#define STATS_INTERVAL_MS     1000
#define STATS_START_DELAY_MS  2000

说明：
- STATS_MAX_TASKS：uxTaskGetSystemState 一次最多采样的任务数；
  当前系统约十几个任务（含 Idle、Timer、TzCtrl），32 足够。
- STATS_INTERVAL_MS：统计周期，1 秒。
- STATS_START_DELAY_MS：启动后等待 2 秒再取基线，
  避开系统初始化、UI 加载、SD 卡挂载等启动阶段的干扰。


3. 增加任务句柄和属性（USER CODE BEGIN Variables 区域）
------------------------------------------------------------

static osThreadId_t StatsTaskHandle;
static const osThreadAttr_t StatsTask_attributes = {
  .name = "Stats",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 1024 * 4
};

说明：
- 优先级 osPriorityLow，与 StorageFlush 相同，低于 AI、LVGL 等业务任务，
  打印统计不会抢占关键任务。
- stack_size 单位是字节，1024 * 4 = 4096 字节。
  两个 TaskStatus_t 数组放在静态区，不占任务栈。


4. 实现 FreeRTOS 运行时统计钩子（USER CODE BEGIN 1 区域）
------------------------------------------------------------

__weak void configureTimerForRunTimeStats(void)
{
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
  /* Do not reset CYCCNT: the trace recorder already uses this counter. */
}

__weak unsigned long getRunTimeCounterValue(void)
{
  return (unsigned long)DWT->CYCCNT;
}

说明：
- FreeRTOS 在启动调度器时调用 configureTimerForRunTimeStats()，
  这里只打开 DWT 的 TRCENA 和 CYCCNTENA，不写 DWT->CYCCNT = 0。
- TraceRecorder 在开机 xTraceInitialize() 时已经清零并启用了 CYCCNT。
  如果这里再次清零，Percepio 的时间轴会出现一次向后跳变，
  同时 FreeRTOS 正在累计的任务时间也会出错。
- getRunTimeCounterValue() 直接返回 32 位 CYCCNT。
  600MHz 下一次回绕约 2^32 / 600000000 = 7.16 秒。


5. 新增 StatsTask（USER CODE BEGIN Application 区域）
------------------------------------------------------------

静态数据：

static TaskStatus_t statsCur[STATS_MAX_TASKS];
static TaskStatus_t statsPrev[STATS_MAX_TASKS];
static UBaseType_t statsPrevCount;

任务流程（完整逻辑）：

static void StatsTask(void *argument)
{
    uint32_t prevCycles;
    uint32_t nowCycles;
    uint32_t totalCycles;
    UBaseType_t count;
    UBaseType_t i;
    UBaseType_t j;
    uint32_t prevCounter;
    uint32_t delta;
    uint32_t pct;
    uint32_t watermark;
    const char *name;

    (void)argument;

    vTaskDelay(pdMS_TO_TICKS(STATS_START_DELAY_MS));
    statsPrevCount = uxTaskGetSystemState(statsPrev, STATS_MAX_TASKS, NULL);
    prevCycles = DWT->CYCCNT;

    for (;;)
    {
        vTaskDelay(pdMS_TO_TICKS(STATS_INTERVAL_MS));

        count = uxTaskGetSystemState(statsCur, STATS_MAX_TASKS, NULL);
        nowCycles = DWT->CYCCNT;
        totalCycles = nowCycles - prevCycles;

        if (totalCycles == 0u)
        {
            prevCycles = nowCycles;
            continue;
        }

        printf("--- %u ms ---\r\n", (unsigned int)STATS_INTERVAL_MS);

        for (i = 0u; i < count; i++)
        {
            prevCounter = statsCur[i].ulRunTimeCounter;
            for (j = 0u; j < statsPrevCount; j++)
            {
                if (statsPrev[j].xHandle == statsCur[i].xHandle)
                {
                    prevCounter = statsPrev[j].ulRunTimeCounter;
                    break;
                }
            }

            delta = statsCur[i].ulRunTimeCounter - prevCounter;
            pct = (uint32_t)(((uint64_t)delta * 100u) / totalCycles);
            watermark = (uint32_t)uxTaskGetStackHighWaterMark(statsCur[i].xHandle)
                        * (uint32_t)sizeof(StackType_t);
            name = statsCur[i].pcTaskName;

            printf("%-16s %3u%%  stack %5u B\r\n",
                   (name != NULL) ? name : "?",
                   (unsigned int)pct,
                   (unsigned int)watermark);
        }

        for (i = 0u; i < count; i++)
        {
            statsPrev[i] = statsCur[i];
        }
        statsPrevCount = count;
        prevCycles = nowCycles;
    }
}


6. 创建 StatsTask（MX_FREERTOS_Init 的 RTOS_THREADS 段）
------------------------------------------------------------

StatsTaskHandle = osThreadNew(StatsTask, NULL, &StatsTask_attributes);

创建位置在 StorageFlushHandle 之后，在 osKernelStart() 之前完成，
调度器启动后任务才会真正运行。

============================================================
四、实现原理
============================================================

1. 时基：DWT CYCCNT
------------------------------------------------------------

Cortex-M55 的 DWT 周期计数器每个 CPU 时钟加 1。
SystemCoreClock = 600MHz 时，1 秒 = 600,000,000 个计数。
FreeRTOS 的运行时统计只需要一个单调递增的无符号计数器，
因此可以直接使用 CYCCNT，不需要额外定时器。

2. FreeRTOS 如何累计每个任务的运行时间
------------------------------------------------------------

任务切换时（PendSV 中的 vTaskSwitchContext），FreeRTOS 会：

    ulTotalRunTime = portGET_RUN_TIME_COUNTER_VALUE();
    pxCurrentTCB->ulRunTimeCounter +=
        (ulTotalRunTime - ulTaskSwitchedInTime);
    ulTaskSwitchedInTime = ulTotalRunTime;

即每次切换时用“当前计数 - 上次切换时的计数”的差值累加到当前任务，
这是无符号减法，单次差值本身是抗回绕的。

uxTaskGetSystemState() 会把每个任务的 ulRunTimeCounter 填进
TaskStatus_t 数组返回，这就是本方案的数据来源。

3. 为什么不能直接用 vTaskGetRunTimeStats()
------------------------------------------------------------

vTaskGetRunTimeStats() 的百分比分母不是“所有任务累计值之和”，
而是直接读取当前原始计数器：

    tasks.c:2597  *pulTotalRunTime = portGET_RUN_TIME_COUNTER_VALUE();
    tasks.c:4653  uxTaskGetSystemState(..., &ulTotalTime);
    tasks.c:4656  ulTotalTime /= 100UL;
    tasks.c:4667  percentage = taskCounter / ulTotalTime;

问题：
- CYCCNT 每 7.16 秒回绕一次；
- 回绕后，分母变成“回绕后的相位”（例如 2.8 秒对应的计数），
  而任务累计计数器仍保持 32 位模值；
- 结果会出现大于 100% 的百分比。

因此本方案不使用 vTaskGetRunTimeStats()。
原优化方案中“回绕不影响百分比”的说法是错误的。

4. 差值法的核心
------------------------------------------------------------

每次统计时保存上一周期的快照：

    totalCycles = nowCycles - prevCycles;
    delta       = taskCounterNow - taskCounterPrev;

因为统计间隔是 1 秒，远小于 7.16 秒的回绕周期：
- 两个差值都在一个回绕周期内；
- 32 位无符号减法的结果天然正确，不需要软件扩展成 64 位；
- 得到的百分比是“最近 1 秒内该任务占用的 CPU 比例”。

公式：

    pct = delta * 100 / totalCycles

5. 为什么乘法要先转成 uint64_t
------------------------------------------------------------

1 秒内 delta 最大约 6 亿（600MHz），
delta * 100 约 600 亿，超过 32 位无符号上限（约 42.9 亿）。
所以代码写为：

    pct = (uint32_t)(((uint64_t)delta * 100u) / totalCycles);

先扩展为 64 位再乘 100，最后除，避免溢出。

6. 为什么按任务 handle 匹配，而不是按数组下标
------------------------------------------------------------

系统中的任务会动态变化：
- AI_Task 由 UI 动作在 actions.c 中动态创建；
- 也可能被删除后重新创建；
- uxTaskGetSystemState 返回的任务顺序不保证稳定。

如果用下标匹配，任务顺序变化时会把 A 任务的上周期计数减到 B 任务上，
出现负值或超过 100% 的异常。
因此用一个嵌套循环，在上一周期快照中按 xHandle 查找同一任务。
新创建的任务找不到上一周期记录时，以当前计数作为基线，
本周期记为 0%，避免把任务生命周期内的累计值算进 1 秒窗口。

7. 栈高水位
------------------------------------------------------------

uxTaskGetStackHighWaterMark(handle) 返回任务从创建以来
剩余栈空间的最小值，单位是 StackType_t 的“字数”。
Cortex-M 上 sizeof(StackType_t) = 4，
因此乘以 4 得到字节数：

    watermark = uxTaskGetStackHighWaterMark(handle) * sizeof(StackType_t);

该值越小说明栈越接近耗尽。
用它验证 AI_Task（8KB 栈）和 LVGL（128KB 栈）的实际余量。

8. 与 Percepio 共存
------------------------------------------------------------

两者的时间基都是 DWT CYCCNT：
- TraceRecorder 在 xTraceInitialize() 中清零并启用 CYCCNT；
- P0-1 的 configureTimerForRunTimeStats() 只重复置位使能位，
  不再清零；
- 所以 Percepio 时间轴不会跳变，运行时统计也能正常工作。

注意：开启 trace 时，每次内核 API 调用和任务切换都有额外开销，
这些开销会被计入各任务占比。
做优化前后对比时，两次测量要么都关 trace，要么都开 trace，
不要一次开一次关。

9. 输出示例
------------------------------------------------------------

--- 1000 ms ---
IDLE               15%  stack   400 B
AI_Task            42%  stack  3120 B
LVGL               35%  stack 95000 B
Stats               1%  stack  3900 B
...

各任务百分比之和接近 100%。
ISR 执行时间会被计入被打断的任务，这是 FreeRTOS 统计的固有口径，
不是代码错误。

============================================================
五、验证方法
============================================================

1. CubeIDE 重新 Build Release（当前 Release elf 是修改前构建的，
   直接下载不会有统计输出），烧录运行。
2. 串口 115200，启动约 3 秒后开始输出
   （2 秒稳定等待 + 1 秒统计间隔）。
3. 连续运行 1 分钟以上，检查：
   - 每秒输出一组；
   - 没有大于 100% 的百分比；
   - 各任务百分比之和约 100%；
   - 栈水位数值稳定，AI_Task / LVGL 有明显余量。
4. 修复 cam_frame_sem 双重 give 前后对比：
   AI_Task 占比应下降，或 IDLE 占比上升。
5. 如果打印本身对系统影响明显，可把 STATS_INTERVAL_MS 改为 5000。

============================================================
六、已知限制
============================================================

1. ISR 时间计入被打断的任务，不能单独看到中断占比。
   中断占比需要用 Percepio 或额外的 GPIO/DWT 测量。
2. 串口输出是阻塞发送（main.c 的 _write 使用 HAL_UART_Transmit，
   超时 0xFFFF）。每秒十几行会增加低优先级任务的占用，
   必要时延长统计周期或精简输出。
3. 统计精度取决于任务切换；百分比是 1 秒窗口的近似值，
   不是逐指令级测量。
4. 如果后续有代码再次复位 CYCCNT，会同时破坏本统计和
   Percepio 时间轴，需要避免。
5. 本统计只回答“谁占用了多少 CPU”，不回答“为什么被阻塞”，
   后者仍用 Percepio 快照分析。

============================================================
七、结论
============================================================

P0-1 已完成：
- 运行时统计钩子生效；
- 每秒输出任务级 CPU 占比和栈高水位；
- 使用差值法规避 CYCCNT 7.16 秒回绕；
- 与 Percepio 共用 DWT 且不破坏其时间轴。

后续优化效果评估建议：
- 用 Percepio 看单帧内的阻塞和唤醒顺序；
- 用本统计看长时间的平均负载、栈余量和优化前后回归。
