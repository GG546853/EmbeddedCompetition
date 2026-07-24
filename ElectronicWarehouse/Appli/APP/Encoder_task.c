#include "Encoder_task.h"
#include "app_freertos.h"
#include "tim.h"
#include "main.h"
#include "imx335.h"
#include "ui_bridge.h"
#include "vars.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "uart.h"

/* ---- parameter ranges ---- */
#define GAIN_MIN              0
#define GAIN_MAX              72000
#define GAIN_STEP             100      /* 100 mdB per detent */

#define EXPOSURE_MIN          0
#define EXPOSURE_MAX          33266
#define EXPOSURE_STEP         100      /* 100 us per detent */

/* WB discrete values: 0=auto, 2856=A, 4000=TL84, 5000=D50, 6500=D65 */
static const uint32_t WB_VALUES[] = { 0, 2856, 4000, 5000, 6500 };
#define WB_COUNT              5

/* button debounce / long-press timing */
#define DEBOUNCE_MS           30
#define LONG_PRESS_MS         800
#define POWER_LONG_PRESS_MS   2000     /* 3 s hold to power off */

/* encoder: 4 counts per detent in TI12 mode */
#define ENC_COUNTS_PER_DETENT 1

/* ---- task handle ---- */
osThreadId_t Encoder_TaskHandle;
const osThreadAttr_t EncoderTask_attributes = {
    .name = "EncoderTask",
    .priority = (osPriority_t) osPriorityNormal + 2,  /* above Sensor_Task(+1) for responsive encoder */
    .stack_size = 1024 * 4
};

void Encoder_Task(void *argument)
{
    (void)argument;

    osDelay(3500);  /* wait for LVGL_Task ui_init() to set g_mainAssets */

    /* restore or init Flow variables */
    int gain     = ui_get_integer(FLOW_GLOBAL_VARIABLE_GAIN);
    int exposure = ui_get_integer(FLOW_GLOBAL_VARIABLE_EXPOSURE);
    int wb       = ui_get_integer(FLOW_GLOBAL_VARIABLE_WB);
    int menu_vis = !ui_get_integer(FLOW_GLOBAL_VARIABLE_MENU_VISIBLE);
    int menu_st  = ui_get_integer(FLOW_GLOBAL_VARIABLE_MENU_STATE);
    int sel_idx  = ui_get_integer(FLOW_GLOBAL_VARIABLE_SELECTED_INDEX);

    if (gain < GAIN_MIN || gain > GAIN_MAX)         gain = 20000;
    if (exposure < EXPOSURE_MIN || exposure > EXPOSURE_MAX) exposure = 15000;
    if (sel_idx < 0 || sel_idx > 2)                 sel_idx = 0;

    /* write corrected defaults back */
    ui_set_integer(FLOW_GLOBAL_VARIABLE_GAIN, gain);
    ui_set_integer(FLOW_GLOBAL_VARIABLE_EXPOSURE, exposure);
    ui_set_integer(FLOW_GLOBAL_VARIABLE_SELECTED_INDEX, sel_idx);

    /* force hidden on startup */
    menu_vis = 0;
    ui_set_integer(FLOW_GLOBAL_VARIABLE_MENU_VISIBLE, !menu_vis);

    /* button state */
    uint8_t  btn_last     = 1;   /* pull-up: 1=released */
    uint8_t  btn_stable   = 1;
    uint32_t btn_press_ms = 0;
    uint32_t btn_deadline = 0;
    uint8_t  long_sent    = 0;

    /* power button state (PD6) */
    uint8_t  pwr_last     = 1;
    uint8_t  pwr_stable   = 1;
    uint32_t pwr_press_ms = 0;
    uint32_t pwr_deadline = 0;

    /* encoder state */
    int16_t  enc_last  = (int16_t)__HAL_TIM_GET_COUNTER(&htim1);
    int16_t  enc_delta;

    /* debug: loop counter for rate-limited prints */
    uint32_t loop_cnt = 0;

    printf("[Encoder] Task started\r\n");
    printf("[Encoder] menu_vis=%d menu_st=%d sel_idx=%d(%s)\r\n",
           menu_vis, menu_st, sel_idx,
           sel_idx == 0 ? "WB" : sel_idx == 1 ? "Gain" : "Exposure");
    printf("[Encoder] WB=%d Gain=%d Exposure=%d\r\n", wb, gain, exposure);

    for (;;) {
        loop_cnt++;

        /* ---- 1. read button (PF1, pull-up, active-low) ---- */
        uint8_t  btn_raw = (uint8_t)HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_1);
        uint32_t now     = osKernelGetTickCount();

        if (btn_raw != btn_last) {
            btn_last = btn_raw;
            btn_deadline = now + pdMS_TO_TICKS(DEBOUNCE_MS);
        }
        if (now >= btn_deadline && btn_raw != btn_stable) {
            btn_stable = btn_raw;
            if (btn_stable == 0) {
                /* press */
                btn_press_ms = now;
                long_sent = 0;
            } else {
                /* release — decide short/long */
                uint32_t held = (now - btn_press_ms) * portTICK_PERIOD_MS;
                printf("[DBG] BTN release held=%lu ms menu_vis=%d\r\n", held, menu_vis);
                if (held >= LONG_PRESS_MS && !long_sent) {
                    menu_vis = !menu_vis;
                    ui_set_integer(FLOW_GLOBAL_VARIABLE_MENU_VISIBLE, !menu_vis);
                    printf("[Encoder] Menu %s\r\n", menu_vis ? "SHOW" : "HIDE");
                } else if (held < LONG_PRESS_MS && held > DEBOUNCE_MS) {
                    if (menu_vis) {
                        menu_st = (menu_st == 0) ? 1 : 0;
                        ui_set_integer(FLOW_GLOBAL_VARIABLE_MENU_STATE, menu_st);
                        printf("[Encoder] State=%d (%s)\r\n", menu_st,
                               menu_st ? "ADJUST" : "SELECT");
                    } else {
                        printf("[DBG] short-press ignored: menu_vis=0\r\n");
                    }
                } else {
                    printf("[DBG] short-press cond fail: held=%lu (need >%d && <%d)\r\n",
                           held, DEBOUNCE_MS, LONG_PRESS_MS);
                }
            }
        }
        /* long-press continuous detection */
        if (btn_stable == 0 && !long_sent) {
            uint32_t held = (now - btn_press_ms) * portTICK_PERIOD_MS;
            if (held >= LONG_PRESS_MS) {
                long_sent = 1;
                menu_vis = !menu_vis;
                ui_set_integer(FLOW_GLOBAL_VARIABLE_MENU_VISIBLE, !menu_vis);
                printf("[Encoder] Menu %s (long)\r\n", menu_vis ? "SHOW" : "HIDE");
            }
        }

        /* ---- 2. read encoder ---- */
        int16_t enc_now = (int16_t)__HAL_TIM_GET_COUNTER(&htim1);
        enc_delta = enc_now - enc_last;
        enc_last  = enc_now;

        /* ---- 3. read power button (PD6, pull-up, active-low) ---- */
        {
            uint8_t  pwr_raw = (uint8_t)HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_6);
            now     = osKernelGetTickCount();

            if (pwr_raw != pwr_last) {
                pwr_last = pwr_raw;
                pwr_deadline = now + pdMS_TO_TICKS(DEBOUNCE_MS);
            }
            if (now >= pwr_deadline && pwr_raw != pwr_stable) {
                pwr_stable = pwr_raw;
                if (pwr_stable == 0) {
                    pwr_press_ms = now;
                }
            }
            /* long-press detection while held */
            if (pwr_stable == 0) {
                uint32_t held = (now - pwr_press_ms) * portTICK_PERIOD_MS;
                if ((loop_cnt % 50) == 0) {
                    printf("[DBG] PWR held=%lu ms\r\n", held);
                }
                if (held >= POWER_LONG_PRESS_MS) {
                    printf("[Power] PD6 long press (%lu ms) — shutting down\r\n", held);
                    printf("[Power] PG5 before=%d\r\n", (int)HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_5));
                    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_5, 0);
                    printf("[Power] PG5 after=%d\r\n", (int)HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_5));
                    for (;;) { vTaskDelay(pdMS_TO_TICKS(100)); }
                }
            }
        }

        if (!menu_vis) {
            vTaskDelay(pdMS_TO_TICKS(10));
            continue;
        }

        if (enc_delta >= ENC_COUNTS_PER_DETENT || enc_delta <= -ENC_COUNTS_PER_DETENT) {
            int steps = enc_delta / ENC_COUNTS_PER_DETENT;
            int dir   = (steps > 0) ? 1 : -1;

            if (menu_st == 0) {
                /* SELECT mode — cycle selected_index */
                sel_idx = (sel_idx + dir + 3) % 3;
                ui_set_integer(FLOW_GLOBAL_VARIABLE_SELECTED_INDEX, sel_idx);
                printf("[Encoder] Select=%d (%s)\r\n", sel_idx,
                       sel_idx == 0 ? "WB" : sel_idx == 1 ? "Gain" : "Exposure");
            } else {
                /* ADJUST mode — modify selected variable */
                switch (sel_idx) {
                case 0: { /* WB — cycle discrete values */
                    int cur = 0;
                    for (int i = 0; i < WB_COUNT; i++) {
                        if (wb == (int)WB_VALUES[i]) { cur = i; break; }
                    }
                    cur = (cur + dir + WB_COUNT) % WB_COUNT;
                    wb = (int)WB_VALUES[cur];
                    ui_set_integer(FLOW_GLOBAL_VARIABLE_WB, wb);
                    imx335_set_wb_mode((uint32_t)wb);
                    break;
                }
                case 1: /* Gain */
                    gain += dir * (int)GAIN_STEP;
                    if (gain < GAIN_MIN) gain = GAIN_MIN;
                    if (gain > GAIN_MAX) gain = GAIN_MAX;
                    ui_set_integer(FLOW_GLOBAL_VARIABLE_GAIN, gain);
                    imx335_set_manual_gain((uint32_t)gain);
                    break;
                case 2: /* Exposure */
                    exposure += dir * (int)EXPOSURE_STEP;
                    if (exposure < EXPOSURE_MIN) exposure = EXPOSURE_MIN;
                    if (exposure > EXPOSURE_MAX) exposure = EXPOSURE_MAX;
                    ui_set_integer(FLOW_GLOBAL_VARIABLE_EXPOSURE, exposure);
                    imx335_set_manual_exposure((uint32_t)exposure);
                    break;
                }
            }
        } else {
            /* stub 4: delta non-zero but below threshold */
            if (enc_delta != 0) {
                printf("[DBG] ENC delta=%d below threshold (+/-%d)\r\n",
                       enc_delta, ENC_COUNTS_PER_DETENT);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10));  /* 100 Hz */
    }
}
