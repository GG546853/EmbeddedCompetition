#include "app_freertos.h"
#include "AI_task.h"
#include "app_x-cube-ai.h"

#include "imx335.h"
#include "dcmipp.h"
#include "network_f.h"
#include "network_fc.h"
#include "main.h"
#include "rgblcd.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "uart.h"
#include "dma2d.h"
#include "storage.h"
#include "UART_protocol.h"
#include "UART4_RxTask.h"
#include "usart.h"
#include "sys.h"
#include "lvgl.h"
#include "ui/screens.h"
#include "ui/ui_bridge.h"
#include "Outbound_task.h"
#include "profiling.h"
#define DISP_W 800
#define DISP_H 480
#define BOX_THICKNESS 3
#define KP_SIZE 5
#define CMD_BUF_SIZE  64

/* Face size gate: only accept detection when face max(w,h) is in this range.
   Calibrate by standing at a comfortable distance and checking the box size
   printed as "[GATE] face_size=..." on serial. */
#define FACE_SIZE_MIN  0.25f
#define FACE_SIZE_MAX  0.75f

/* Model 2 (face recognition) only runs during enrollment, not during idle. */

extern uint8_t g_ltdc_ui_framebuf[800 * 480 * 2];   /* LTDC 图层 1，LVGL 共用 */

#define UI565(r, g, b)   ((uint16_t)((((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | ((b) >> 3)))
#define BOX_COLOR565     UI565(0, 255, 0)   /* 与原来 RGB888 (0,255,0) 同色 */
#define OVERLAY_CLEAR    0x0000u            /* 颜色键=黑色 → 透明 */
#define OVERLAY_RECT_MAX 16                 /* AI_FD_MAX_DETECTIONS=1 → 4 边 + 6 关键点 = 10 */

volatile int g_trigger_identify = 0;
volatile int g_identify_done  = 0;
volatile int g_identify_match = 0;
volatile int g_identify_index = 0;
volatile int g_auto_unlock    = 0;  /* 1 = auto-execute unlock on next poll */
char         g_identify_name[FACE_NAME_MAX];
float        g_identify_dist;
static uint16_t g_pending_face_code;

volatile int g_enroll_progress = 0;
volatile int g_enroll_total    = FACE_ENROLL_SAMPLES;
volatile int g_enroll_state    = 0;  /* 0=idle, 1=enrolling, 2=done, 3=waiting_name */
char         g_enroll_msg[64]  = {0};

static int16_t s_rect[OVERLAY_RECT_MAX][4];   /* 上一帧画过的矩形，用于擦除 */
static uint8_t s_rect_n;

/* 画一个矩形到图层 1；record=1 时把裁剪后的矩形记入擦除表 */
static void ui_edge(int x, int y, int w, int h, uint16_t color, int record)
{
    if (x < 0) { w += x; x = 0; }
    if (y < 0) { h += y; y = 0; }
    if (x + w > DISP_W) w = DISP_W - x;
    if (y + h > DISP_H) h = DISP_H - y;
    if (w <= 0 || h <= 0) return;

    if (record && s_rect_n < OVERLAY_RECT_MAX) {
        s_rect[s_rect_n][0] = (int16_t)x;
        s_rect[s_rect_n][1] = (int16_t)y;
        s_rect[s_rect_n][2] = (int16_t)w;
        s_rect[s_rect_n][3] = (int16_t)h;
        s_rect_n++;
    }

    extern osMutexId_t dma2d_mutex;
    if (dma2d_mutex != NULL) osMutexAcquire(dma2d_mutex, osWaitForever);

    hdma2d.Init.Mode = DMA2D_R2M;
    hdma2d.Init.ColorMode = DMA2D_OUTPUT_RGB565;
    hdma2d.Init.OutputOffset = DISP_W - w;
    HAL_DMA2D_Init(&hdma2d);
    HAL_DMA2D_Start(&hdma2d, color,
                    (uint32_t)&g_ltdc_ui_framebuf[(y * DISP_W + x) * 2], w, h);
    HAL_DMA2D_PollForTransfer(&hdma2d, 50);

    if (dma2d_mutex != NULL) osMutexRelease(dma2d_mutex);
}

static void ui_box(int x, int y, int w, int h, uint16_t color, int record)
{
    ui_edge(x, y, w, BOX_THICKNESS, color, record);                     /* top */
    ui_edge(x, y + h - BOX_THICKNESS, w, BOX_THICKNESS, color, record); /* bottom */
    ui_edge(x, y, BOX_THICKNESS, h, color, record);                     /* left */
    ui_edge(x + w - BOX_THICKNESS, y, BOX_THICKNESS, h, color, record); /* right */
}

static void ui_kp(int x, int y, int record)
{
    ui_edge(x - KP_SIZE / 2, y - KP_SIZE / 2, KP_SIZE, KP_SIZE, BOX_COLOR565, record);
}

/* PIPE2 center-crop compensation: NN sees sensor columns [324,2268] of full [0,2592].
   On the 800-wide display this region maps to x ∈ [100, 700].
   x_display = x_nn * (1944/2592 * 800) + (324/2592 * 800) = x_nn * 600 + 100 */
#define NN2DISP_SCALE_X  600.0f
#define NN2DISP_OFFSET_X 100.0f

static void draw_one_detection(ai_detection_t *d)
{
    int bx = (int)(d->x_center * NN2DISP_SCALE_X + NN2DISP_OFFSET_X
                   - d->width  * NN2DISP_SCALE_X * 0.5f);
    int by = (int)(d->y_center * DISP_H - d->height * DISP_H * 0.5f);
    int bw = (int)(d->width  * NN2DISP_SCALE_X);
    int bh = (int)(d->height * DISP_H);

    ui_box(bx, by, bw, bh, BOX_COLOR565, 1);

    for (int k = 0; k < 6; k++) {
        int kx = (int)(d->keypoints[k][0] * NN2DISP_SCALE_X + NN2DISP_OFFSET_X);
        int ky = (int)(d->keypoints[k][1] * DISP_H);
        ui_kp(kx, ky, 1);
    }
}

static void draw_detections_on_display(ai_result_t *result)
{
    uint64_t t = profiling_now();
    uint8_t n = s_rect_n;
    s_rect_n = 0;

    /* 先擦掉上一帧的框（填黑色 = 恢复颜色键透明） */
    for (uint8_t i = 0; i < n; i++) {
        ui_edge(s_rect[i][0], s_rect[i][1], s_rect[i][2], s_rect[i][3], OVERLAY_CLEAR, 0);
    }
    /* 再画本帧；nb_detect==0 时就只剩擦除 */
    for (uint32_t i = 0; i < result->nb_detect; i++) {
        draw_one_detection(&result->detections[i]);
    }
    profiling_stage(PROF_DRAW, profiling_now() - t);
}

void ai_clear_overlay(void)
{
    uint8_t n = s_rect_n;
    s_rect_n = 0;
    for (uint8_t i = 0; i < n; i++) {
        ui_edge(s_rect[i][0], s_rect[i][1], s_rect[i][2], s_rect[i][3], OVERLAY_CLEAR, 0);
    }
}

osThreadId_t AITaskHandle;
const osThreadAttr_t AITask_attributes = {
    .name = "AITask",
    .priority = (osPriority_t) osPriorityNormal + 1,
    .stack_size = 2048 * 4
};

extern DCMIPP_HandleTypeDef hdcmipp;
extern uint8_t *buffer_in;
extern uint8_t *buffer_in_fc;

/* Intermediate buffer for DCMIPP PIPE2 uint8 RGB888 output.
   Must be in .noncacheable section and 32-byte aligned for DMA access. */
uint8_t nn_input_u8[128 * 128 * 3] __attribute__((section(".noncacheable"), aligned(32)));

/* Dedicated face crop buffer — must NOT alias nn_input_u8 (PIPE2 overwrites it). */
static uint8_t face_crop_buf[112 * 112 * 3]
    __attribute__((section(".noncacheable"), aligned(32)));
#define FACE_CROP_BUF  face_crop_buf

static const char *kp_names[6] = {
    "LEye", "REye", "Nose", "Mouth", "LEar", "REar"
};
extern osSemaphoreId_t cam_frame_sem;

/* -------------------------------------------------------------------------- */
/*                     Serial Command Parser (uses BSP RX buffer)               */
/* -------------------------------------------------------------------------- */

typedef enum {
    REID_IDLE,
    REID_REGISTER,
} reid_action_t;

int g_model2_enabled = 0;  /* 1 = Model 2 runs (identify or enroll), 0 = off */
reid_action_t reid_pending;
static char          reid_name[FACE_NAME_MAX];
int           g_capture_pending = 0;

void ai_start_enroll(const char *name) {
    strncpy(reid_name, name, FACE_NAME_MAX - 1);
    reid_name[FACE_NAME_MAX - 1] = '\0';
    reid_pending = REID_REGISTER;
    g_capture_pending = 1;
    g_enroll_progress = 0;
    g_enroll_state = 1;
    snprintf(g_enroll_msg, sizeof(g_enroll_msg), "Enroll sample 0/%d", FACE_ENROLL_SAMPLES);
    printf("[REID] Enrolling '%s' — auto-capture enabled\r\n", reid_name);
}

/* 1:1 verification target (loaded by 'select <name>') */
static face_entry_t g_selected_entry;
static int          g_has_selection = 0;
static int          g_auto_dump   = 0;
static int          g_dump_once   = 0;
static int          g_enroll_dump = 0;

static void send_face_dump(void);
static void send_enroll_face_dump(int sample_idx, int total);

/* BSP interrupt-driven RX accumulates into g_uart_rx_buf[].
   g_uart_rx_sta bit15 = line ready (received \r\n). */

static void process_serial_commands(void)
{
    if (!(g_uart_rx_sta & 0x8000)) return;

    uint16_t len = g_uart_rx_sta & 0x3FFF;
    if (len >= CMD_BUF_SIZE) len = CMD_BUF_SIZE - 1;

    char cmd[CMD_BUF_SIZE];
    memcpy(cmd, g_uart_rx_buf, len);
    cmd[len] = '\0';

    g_uart_rx_sta = 0;  /* reset for next line */

    printf("[CMD] %s\r\n", cmd);

    if (strncmp(cmd, "register ", 9) == 0) {
        strncpy(reid_name, cmd + 9, FACE_NAME_MAX - 1);
        reid_name[FACE_NAME_MAX - 1] = '\0';
        reid_pending = REID_REGISTER;
        g_capture_pending = 1;
        g_model2_enabled = 1;
        printf("[REID] Enrolling '%s' — send 's' for next sample\r\n", reid_name);
    }
    else if (strcmp(cmd, "s") == 0) {
        if (reid_pending == REID_REGISTER) {
            g_capture_pending = 1;
            printf("[REID] Capture triggered\r\n");
        } else {
            printf("[REID] Not enrolling — use 'register <name>' first\r\n");
        }
    }
    else if (strcmp(cmd, "identify") == 0) {
        printf("[REID] Identification is continuous — results print every frame.\r\n");
    }
    else if (strncmp(cmd, "select ", 7) == 0) {
        char *sel_name = cmd + 7;
        while (*sel_name == ' ') sel_name++;
        if (ai_face_select(sel_name, &g_selected_entry) == 0) {
            g_has_selection = 1;
            printf("[REID] Selected '%s' for 1:1 verification\r\n", sel_name);
        } else {
            printf("[REID] '%s' not found in gallery\r\n", sel_name);
        }
    }
    else if (strcmp(cmd, "unselect") == 0) {
        g_has_selection = 0;
        memset(&g_selected_entry, 0, sizeof(g_selected_entry));
        printf("[REID] Selection cleared, back to 1:N identify\r\n");
    }
    else if (strcmp(cmd, "gallery") == 0) {
        ai_face_gallery_print();
    }
    else if (strcmp(cmd, "clearface") == 0) {
        face_entry_t empty[FACE_GALLERY_MAX] = {0};
        ai_face_gallery_import(empty, 0);
        printf("[TEST] Gallery cleared in RAM (SD NAND NOT modified)\r\n");
    }
    else if (strcmp(cmd, "cam status") == 0) {
        imx335_print_status();
    }
    else if (strncmp(cmd, "cam gain ", 9) == 0) {
        imx335_set_manual_gain((uint32_t)atoi(cmd + 9));
    }
    else if (strncmp(cmd, "cam exp ", 8) == 0) {
        imx335_set_manual_exposure((uint32_t)atoi(cmd + 8));
    }
    else if (strncmp(cmd, "cam wb ", 7) == 0) {
        imx335_set_wb_mode((uint32_t)atoi(cmd + 7));
    }
    else if (strncmp(cmd, "cam auto ", 9) == 0) {
        imx335_set_auto_mode((uint8_t)atoi(cmd + 9));
    }
    else if (strcmp(cmd, "saveface") == 0) {
        storage_save_faces();
    }
    else if (strcmp(cmd, "loadface") == 0) {
        storage_load_faces();
    }
    else if (strcmp(cmd, "saveinv") == 0) {
        storage_save_inventory();
        printf("[TEST] Inventory saved to SD NAND\r\n");
    }
    else if (strcmp(cmd, "loadinv") == 0) {
        storage_load_inventory();
        ui_push_inventory(inventory_item_T, 28, inventory_item_D, 6);
        printf("[TEST] Inventory loaded from SD NAND\r\n");
    }
    else if (strcmp(cmd, "clearinv") == 0) {
        memset(inventory_item_T, 0, sizeof(inventory_item_T));
        memset(inventory_item_D, 0, sizeof(inventory_item_D));
        ui_push_inventory(inventory_item_T, 28, inventory_item_D, 6);
        printf("[TEST] Inventory + history cleared in RAM (SD NAND NOT modified)\r\n");
    }
    else if (strcmp(cmd, "bench info") == 0) {
        profiling_bench_info();
    }
    else if (strcmp(cmd, "bench canary") == 0) {
        profiling_bench_canary();
    }
    else if (strncmp(cmd, "bench read", 10) == 0) {
        uint32_t kb = (uint32_t)atoi(cmd + 10);
        profiling_bench_read(kb ? kb : 1024u, 5u);
    }
    else if (strncmp(cmd, "bench write", 11) == 0) {
        uint32_t kb = (uint32_t)atoi(cmd + 11);
        profiling_bench_write(kb ? kb : 1024u, 5u);
    }
    else if (strncmp(cmd, "bench copy", 10) == 0) {
        uint32_t kb = (uint32_t)atoi(cmd + 10);
        profiling_bench_copy(kb ? kb : 512u, 5u);
    }
    else if (strcmp(cmd, "bench chase") == 0) {
        profiling_bench_chase(8192u);
    }
    else if (strcmp(cmd, "bench matrix") == 0) {
        profiling_bench_matrix();
    }
    else if (strncmp(cmd, "bench infer", 11) == 0) {
        char *p = cmd + 11;
        uint32_t n = (uint32_t)strtoul(p, &p, 10);
        uint32_t cfg = (uint32_t)strtoul(p, &p, 10);
        profiling_bench_infer(n, cfg);
    }
    else if (strncmp(cmd, "bench reid", 10) == 0) {
        char *p = cmd + 10;
        uint32_t n = (uint32_t)strtoul(p, &p, 10);
        uint32_t cfg = (uint32_t)strtoul(p, &p, 10);
        profiling_bench_reid(n, cfg);
    }
    else if (strcmp(cmd, "help") == 0) {
        printf("[HELP] Commands:\r\n");
        printf("  register <name>  -- start enrolling current face\r\n");
        printf("  s                -- capture one sample (during enrollment)\r\n");
        printf("  select <name>    -- select identity for 1:1 verification\r\n");
        printf("  unselect         -- clear selection, back to 1:N\r\n");
        printf("  identify         -- identify/verify current face\r\n");
        printf("  gallery          -- list registered faces\r\n");
        printf("  saveface         -- save face gallery to SD NAND\r\n");
        printf("  loadface         -- load face gallery from SD NAND\r\n");
        printf("  clearface        -- clear face gallery in RAM only\r\n");
        printf("  saveinv          -- save inventory to SD NAND\r\n");
        printf("  loadinv          -- load inventory from SD NAND\r\n");
        printf("  clearinv         -- clear inventory in RAM only\r\n");
        printf("  --- Camera ---\r\n");
        printf("  cam status       -- print camera Gain/Exposure/WB/AEC/AWB\r\n");
        printf("  cam gain <val>   -- set manual gain 0~72000 mdB\r\n");
        printf("  cam exp <val>    -- set manual exposure 0~33266 us\r\n");
        printf("  cam wb <temp>    -- WB: 0=auto, 2856=A, 4000=TL84, 5000=D50, 6500=D65\r\n");
        printf("  cam auto <0|1>   -- 0=manual, 1=auto (AEC+AWB)\r\n");
        printf("  crccc            -- CRC32 check of network_fc weights\r\n");
        printf("  --- Dump ---\r\n");
        printf("  dumpface         -- one-shot hex dump of face crop\r\n");
        printf("  dumpface on/off  -- auto-dump on every identify frame\r\n");
        printf("  enrolldump on/off-- auto-dump on every enrollment capture\r\n");
        printf("  --- HyperRAM bench (measurement only) ---\r\n");
        printf("  bench info       -- XSPI1 clock/registers + cache geometry\r\n");
        printf("  bench canary     -- probe 0x90600000 writability\r\n");
        printf("  bench read [KB]  -- sequential read bandwidth (default 1024)\r\n");
        printf("  bench write [KB] -- sequential write bandwidth\r\n");
        printf("  bench copy [KB]  -- read+write copy bandwidth (default 512)\r\n");
        printf("  bench chase      -- dependent-load latency (HyperRAM vs SRAM)\r\n");
        printf("  bench matrix     -- C0..C4 load ladder (toggles camera/LTDC)\r\n");
        printf("  bench infer [n] [cfg] -- detection inference (cfg 1=off 2=on)\r\n");
        printf("  bench reid [n] [cfg]  -- fc inference timing\r\n");
    }
    else if (strcmp(cmd, "crccc") == 0) {
        extern uint32_t fc_weight_crc32(void);
        uint32_t crc = fc_weight_crc32();
        printf("[CRC-FC] 0x71200000 + 1156289 bytes -> CRC32=0x%08lX (expect 0xF3588EFA)\r\n",
               (unsigned long)crc);
    }
    else if (strcmp(cmd, "dumpface") == 0) {
        g_dump_once = 1;
    }
    else if (strcmp(cmd, "dumpface on") == 0) {
        g_auto_dump = 1;
        printf("[DUMP] Auto-dump ON — face crop sent on every identify\r\n");
    }
    else if (strcmp(cmd, "dumpface off") == 0) {
        g_auto_dump = 0;
        printf("[DUMP] Auto-dump OFF\r\n");
    }
    else if (strcmp(cmd, "enrolldump on") == 0) {
        g_enroll_dump = 1;
        printf("[DUMP] Enroll-dump ON — face crop sent on every enrollment capture\r\n");
    }
    else if (strcmp(cmd, "enrolldump off") == 0) {
        g_enroll_dump = 0;
        printf("[DUMP] Enroll-dump OFF\r\n");
    }
}

/* -------------------------------------------------------------------------- */
/*                     Face Recognition Pipeline (continuous)                  */
/* -------------------------------------------------------------------------- */

static void send_face_dump(void)
{
    const uint32_t dump_size = 112 * 112 * 3;  /* 37632 bytes RGB */

    /* Ensure face_crop_buf (written by CPU software resize) is flushed to physical memory */
    SCB_CleanDCache_by_Addr((uint32_t *)FACE_CROP_BUF, dump_size);
    __DSB();

    printf("[DUMP-FACE] %lu bytes hex follow...\r\n", (unsigned long)dump_size);

    /* 16 pixels (48 hex chars) per line */
    for (uint32_t i = 0; i < dump_size; i += 48) {
        uint32_t chunk = (i + 48 <= dump_size) ? 48 : (dump_size - i);
        for (uint32_t j = 0; j < chunk; j++) {
            printf("%02X", FACE_CROP_BUF[i + j]);
        }
        printf("\r\n");
    }

    printf("[DUMP-FACE] done\r\n");
}

static void send_enroll_face_dump(int sample_idx, int total)
{
    const uint32_t dump_size = 112 * 112 * 3;

    SCB_CleanDCache_by_Addr((uint32_t *)FACE_CROP_BUF, dump_size);
    __DSB();

    printf("[DUMP-ENROLL] sample %d/%d %lu bytes hex follow...\r\n",
           sample_idx, total, (unsigned long)dump_size);

    for (uint32_t i = 0; i < dump_size; i += 48) {
        uint32_t chunk = (i + 48 <= dump_size) ? 48 : (dump_size - i);
        for (uint32_t j = 0; j < chunk; j++) {
            printf("%02X", FACE_CROP_BUF[i + j]);
        }
        printf("\r\n");
    }

    printf("[DUMP-ENROLL] sample %d/%d done\r\n", sample_idx, total);
}

static void run_reid_pipeline(ai_result_t *result)
{
    uint64_t t;

    if (!g_model2_enabled) return;
    if (result->nb_detect == 0) return;

    if (reid_pending == REID_REGISTER) {
        if (!g_capture_pending && !g_dump_once) return;
    } else {
        /* Identify mode: only run once when triggered */
        if (!g_trigger_identify) return;
    }

    ai_detection_t *best = &result->detections[0];

    /* Crop face from LTDC framebuffer */
    HAL_DCMIPP_CSI_PIPE_Stop(&hdcmipp, DCMIPP_PIPE1, DCMIPP_VIRTUAL_CHANNEL0);
    t = profiling_now();
    ai_crop_resize_face_112((uint8_t *)g_ltdc_framebuf, best, FACE_CROP_BUF);
    profiling_stage(PROF_CROP, profiling_now() - t);
    HAL_DCMIPP_CSI_PIPE_Start(&hdcmipp, DCMIPP_PIPE1, DCMIPP_VIRTUAL_CHANNEL0,
                               (uint32_t)g_ltdc_framebuf, DCMIPP_MODE_CONTINUOUS);

    /* Quantize + cache flush + inference */
    quantize_u8_to_s8(FACE_CROP_BUF, (int8_t *)buffer_in_fc,
                      LL_ATON_NETWORK_FC_IN_1_SIZE_BYTES);
    SCB_CleanInvalidateDCache_by_Addr(
        (uint32_t *)buffer_in_fc, LL_ATON_NETWORK_FC_IN_1_SIZE_BYTES);

    float embedding[FACE_EMBEDDING_DIM];
    t = profiling_now();
    ai_face_reid_run(embedding);
    profiling_stage(PROF_FC_RUN, profiling_now() - t);

    if (reid_pending == REID_REGISTER) {
        /* --- Enrollment --- */
        if (g_capture_pending) {
        g_capture_pending = 0;
        int remaining = ai_face_enroll_multi(embedding, reid_name);
        if (remaining != ENROLL_QUALITY_REJECT && g_enroll_dump) {
            send_enroll_face_dump(FACE_ENROLL_SAMPLES - remaining, FACE_ENROLL_SAMPLES);
        }
        if (remaining == ENROLL_QUALITY_REJECT) {
            snprintf(g_enroll_msg, sizeof(g_enroll_msg), "Keep still...");
        }
        else if (remaining == 0) {
            reid_pending = REID_IDLE;
            storage_mark_faces_dirty();
            printf("[REID] Captured %d/%d\r\n", FACE_ENROLL_SAMPLES, FACE_ENROLL_SAMPLES);
            printf("[REID] Enrolled '%s'\r\n", reid_name);

            g_enroll_state = 3;
            snprintf(g_enroll_msg, sizeof(g_enroll_msg), "Waiting for a name");
            {
                char user_id[32];
                uint32_t idx = ai_face_gallery_get_count() - 1;
                g_pending_face_code = sys_rng_get_4digit();
                snprintf(user_id, sizeof(user_id), "user_%03lu:%04u",
                         idx + 1, g_pending_face_code);
                uart4_send_face_enroll(user_id);
                printf("[FACE] Sent enroll: %s, waiting for name...\r\n", user_id);

                g_face_name_ready = 0;
                int timeout = 9000;
                while (!g_face_name_ready && timeout > 0) {
                    osDelay(10);
                    timeout--;
                }
                if (g_face_name_ready) {
                    snprintf(g_enroll_msg, sizeof(g_enroll_msg), "%s",
                             g_face_name_result);
                } else {
                    snprintf(g_enroll_msg, sizeof(g_enroll_msg), "%s", reid_name);
                }
                g_enroll_state = 2;
            }
        }
        else {
            g_enroll_progress = FACE_ENROLL_SAMPLES - remaining;
            snprintf(g_enroll_msg, sizeof(g_enroll_msg),
                     "Enroll sample %d/%d", g_enroll_progress, FACE_ENROLL_SAMPLES);
            printf("[REID] Captured %d/%d\r\n", g_enroll_progress, FACE_ENROLL_SAMPLES);
        }
        } // if (g_capture_pending)
    } else {
        /* --- K-of-N Voting Identify --- */
        if (g_auto_unlock) return;

        static int vote_pass = 0;
        static int vote_total = 0;

        float dist;
        char name[FACE_NAME_MAX];
        int ok;

        if (g_has_selection) {
            ok = ai_face_verify(embedding, &g_selected_entry, &dist);
            if (ok) {
                strncpy(name, g_selected_entry.name, FACE_NAME_MAX);
            }
        } else {
            ok = (ai_face_identify(embedding, name, &dist) >= 0);
        }

        vote_total++;
        if (ok) {
            vote_pass++;
            g_identify_dist = dist;
            strncpy(g_identify_name, name, FACE_NAME_MAX);
            printf("[REID] vote %d/%d: %s dist=%.3f\r\n",
                   vote_total, VOTE_N, name, dist);
        } else {
            printf("[REID] vote %d/%d: unknown dist=%.3f\r\n",
                   vote_total, VOTE_N, dist);
        }
        {
            char buf[16];
            snprintf(buf, sizeof(buf), "%.3f", dist);
            ui_set_cam_text(buf);
        }

        if (vote_total >= VOTE_N) {
            g_identify_match = (vote_pass >= VOTE_K) ? 1 : 0;
            if (g_identify_match) {
                printf("[REID] Vote %d/%d PASS\r\n", vote_pass, VOTE_N);
            } else {
                printf("[REID] Vote %d/%d FAIL\r\n", vote_pass, VOTE_N);
            }
            g_identify_done = 1;
            g_trigger_identify = 0;
            vote_pass = 0;
            vote_total = 0;
        }
    }
    if (g_auto_dump || g_dump_once) { send_face_dump(); g_dump_once = 0; }
}

/* -------------------------------------------------------------------------- */
/*                               AI Task                                       */
/* -------------------------------------------------------------------------- */

void AI_Task(void *argument)
{
    ai_result_t result;

    /* Wait for camera PIPE1 to be running (Sensor_Task starts it) */
    vTaskDelay(pdMS_TO_TICKS(1000));

    printf("\r\n[REID] Face Re-ID ready. Commands: register <name>, identify, gallery, help\r\n");
    s_rect_n = 0;
    profiling_reset();

    for (;;) {
        uint64_t t;
        profiling_iter();

        t = profiling_now();
        if(osSemaphoreAcquire(cam_frame_sem, HAL_MAX_DELAY) != osOK)
            while(1);
        profiling_stage(PROF_WAIT, profiling_now() - t);

        /* Cache maintenance: DCMIPP wrote to nn_input_u8 via DMA */
        t = profiling_now();
        SCB_CleanInvalidateDCache_by_Addr(
            (uint32_t *)nn_input_u8, sizeof(nn_input_u8));
        profiling_stage(PROF_CACHE_IN, profiling_now() - t);

        /* Software uint8→float32 normalization with BGR→RGB swap.
           DCMIPP outputs BGR order; model was trained on RGB. */
        t = profiling_now();
        float *fin = (float *)buffer_in;
        for (uint32_t i = 0; i < 128 * 128 * 3; i += 3) {
            fin[i + 0] = (float)nn_input_u8[i + 2] / 255.0f;  /* R ← B */
            fin[i + 1] = (float)nn_input_u8[i + 1] / 255.0f;  /* G ← G */
            fin[i + 2] = (float)nn_input_u8[i + 0] / 255.0f;  /* B ← R */
        }
        profiling_stage(PROF_PRE, profiling_now() - t);

        /* Cache maintenance: CPU wrote to buffer_in */
        t = profiling_now();
        SCB_CleanInvalidateDCache_by_Addr(
            (uint32_t *)buffer_in, LL_ATON_NETWORK_F_IN_1_SIZE_BYTES);
        profiling_stage(PROF_CACHE_OUT, profiling_now() - t);

        /* Run NPU inference + post-processing */
        t = profiling_now();
        memset(&result, 0, sizeof(result));
        MX_X_CUBE_AI_Process_User(&result);
        profiling_stage(PROF_NPU_F, profiling_now() - t);

        /* Face size gate: temporarily disabled for PC comparison.
           Out-of-range → no box drawn, no ReID, no identify. */
#if 0
        if (result.nb_detect > 0) {
            float fs = (result.detections[0].width > result.detections[0].height)
                       ? result.detections[0].width
                       : result.detections[0].height;
            if (fs < FACE_SIZE_MIN || fs > FACE_SIZE_MAX) {
                static float last_fs;
                if ((fs < last_fs - 0.005f) || (fs > last_fs + 0.005f)) {
                    printf("[GATE] face_size=%.2f out of [%.2f, %.2f]\r\n",
                           fs, FACE_SIZE_MIN, FACE_SIZE_MAX);
                    last_fs = fs;
                }
                if (reid_pending == REID_REGISTER) {
                    snprintf(g_enroll_msg, sizeof(g_enroll_msg),
                             fs < FACE_SIZE_MIN ? "Too far from the camera"
                                                : "Too close to the camera");
                }
                result.nb_detect = 0;
            }
        }
#endif

#if 0
        /* Print results via serial */
        printf("--- Frame ---\r\n");
        printf("Detections: %lu\r\n", result.nb_detect);
        for (uint32_t i = 0; i < result.nb_detect; i++) {
            ai_detection_t *d = &result.detections[i];
            printf("  [%lu] conf=%.2f box=(%.3f,%.3f,%.3f,%.3f)\r\n",
                   i, d->confidence,
                   d->x_center, d->y_center, d->width, d->height);
            printf("      kp: ");
            for (int k = 0; k < 6; k++) {
                printf("%s(%.3f,%.3f)%s",
                       kp_names[k], d->keypoints[k][0], d->keypoints[k][1],
                       (k < 5) ? " " : "\r\n");
            }
        }
#endif

        /* Check serial commands */
        t = profiling_now();
        process_serial_commands();
        profiling_stage(PROF_CMD, profiling_now() - t);

        /* Run face recognition (Model 2) every frame when face detected */
        t = profiling_now();
        run_reid_pipeline(&result);
        profiling_stage(PROF_REID, profiling_now() - t);

        /* Draw detection boxes + keypoints (draw stage timed inside) */
        draw_detections_on_display(&result);

        HAL_GPIO_WritePin(GPIOG,GPIO_PIN_10,0);
        t = profiling_now();
        vTaskDelay(pdMS_TO_TICKS(1));
        profiling_stage(PROF_TAIL, profiling_now() - t);
    }
}

/* Select a face from the gallery by index for 1:1 verification.
   Called from LVGL dropdown event handler on the lock screen. */
void ai_select_face_by_index(int idx)
{
    uint32_t count = ai_face_gallery_get_count();
    if (idx < 0 || (uint32_t)idx >= count) return;

    face_entry_t entries[FACE_GALLERY_MAX];
    uint32_t n = 0;
    ai_face_gallery_export(entries, &n);
    if (idx >= (int)n) return;

    ai_face_select(entries[idx].name, &g_selected_entry);
    g_has_selection = 1;
    printf("[REID] Selected '%s' for 1:1 verification\r\n", entries[idx].name);
}

/* Refresh the LVGL dropdown list with currently enrolled faces.
   Called after UI init and after enrollment completes. */
void ai_refresh_dropdown_users(void)
{
    face_entry_t entries[FACE_GALLERY_MAX];
    uint32_t count = 0;
    ai_face_gallery_export(entries, &count);

    /* Build dropdown string: "Choose ID\nName1\nName2\n..." */
    static char opts[512];
    strcpy(opts, "Choose ID");
    for (uint32_t i = 0; i < count; i++) {
        strcat(opts, "\n");
        strcat(opts, entries[i].name);
    }

    lv_dropdown_set_options(objects.dropdown_users, opts);
    lv_dropdown_set_selected(objects.dropdown_users, 0);
}
