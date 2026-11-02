# STM32 / CubeMX 踩坑记录

以下问题在本项目开发中实际遇到过，记录备查。

## 1. CubeMX 重新生成后 adc.c 可能未同步

CubeMX 重新生成代码后，`adc.h` 已更新但 `adc.c` 可能还是旧的（如 `adc.h` 声明 ADC2，`adc.c` 仍是 ADC1）。需要手动确认 `adc.c` 也正确更新，否则 Release 有缓存 `.o` 能过、Debug clean build 就炸。

## 2. ADC 必须校准

STM32 ADC 必须调用 `HAL_ADCEx_Calibration_Start` 校准，否则 `HAL_ADC_GetValue` 读到 0。校准放在 `MX_ADCx_Init` 的 `USER CODE Init_2` 区域，不要漏。

## 3. EEZ-Flow 的 stopScript() 导致卡死

EEZ-Flow 的 `stopScript()` 默认是 `assert(false)`。UI 事件/变量变化触发 flow action 执行完毕时会调用 `stopScriptHook() -> stopScript()` 导致卡死。嵌入式项目不跑 flow 脚本，改成空函数即可。
