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
#include "screens.h"
#include "lvgl.h"
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

/* Multi-frame identify: average N embeddings before comparing to gallery.
   Reduces single-frame noise from int8 quantization. ~30fps → 5 frames ≈ 167ms. */
#define IDENTIFY_FRAMES  10

extern uint8_t g_ltdc_framebuf[480 * 800 * 3];

#define RGB888(r, g, b)  (((uint32_t)(r) << 16) | ((uint32_t)(g) << 8) | (uint32_t)(b))

volatile int g_trigger_identify = 0;
volatile int g_identify_done  = 0;
volatile int g_identify_match = 0;
volatile int g_identify_index = -1;
char         g_identify_name[FACE_NAME_MAX];
float        g_identify_dist;
static uint16_t g_pending_face_code;

volatile int g_enroll_progress = 0;
volatile int g_enroll_total    = FACE_ENROLL_SAMPLES;
volatile int g_enroll_state    = 0;  /* 0=idle, 1=enrolling, 2=done, 3=waiting_name */
char         g_enroll_msg[64]  = {0};

static void fill_rect(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b)
{
    if (x < 0) { w += x; x = 0; }
    if (y < 0) { h += y; y = 0; }
    if (x + w > DISP_W) w = DISP_W - x;
    if (y + h > DISP_H) h = DISP_H - y;
    if (w <= 0 || h <= 0) return;

    extern osMutexId_t dma2d_mutex;
    if (dma2d_mutex != NULL) {
        osMutexAcquire(dma2d_mutex, osWaitForever);
    }
    hdma2d.Init.Mode = DMA2D_R2M;
    hdma2d.Init.ColorMode = DMA2D_OUTPUT_RGB888;
    hdma2d.Init.OutputOffset = DISP_W - w;
    HAL_DMA2D_Init(&hdma2d);
    HAL_DMA2D_Start(&hdma2d, RGB888(r, g, b),
                    (uint32_t)&g_ltdc_framebuf[(y * DISP_W + x) * 3], w, h);
    HAL_DMA2D_PollForTransfer(&hdma2d, 50);
    if (dma2d_mutex != NULL) {
        osMutexRelease(dma2d_mutex);
    }
}

static void draw_box(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b)
{
    fill_rect(x, y, w, BOX_THICKNESS, r, g, b);               /* top */
    fill_rect(x, y + h - BOX_THICKNESS, w, BOX_THICKNESS, r, g, b); /* bottom */
    fill_rect(x, y, BOX_THICKNESS, h, r, g, b);               /* left */
    fill_rect(x + w - BOX_THICKNESS, y, BOX_THICKNESS, h, r, g, b); /* right */
}

static void draw_kp(int x, int y, uint8_t r, uint8_t g, uint8_t b)
{
    fill_rect(x - KP_SIZE/2, y - KP_SIZE/2, KP_SIZE, KP_SIZE, r, g, b);
}

/* DCMIPP 每帧覆盖整个缓冲区，旧检测框自然被擦除 */
static void draw_one_detection(ai_detection_t *d, uint8_t r, uint8_t g, uint8_t b)
{
    int bx = (int)(d->x_center * DISP_W - d->width  * DISP_W * 0.5f); // + DET_BOX_OFFSET_X
    int by = (int)(d->y_center * DISP_H - d->height * DISP_H * 0.5f); // + DET_BOX_OFFSET_Y
    int bw = (int)(d->width  * DISP_W);
    int bh = (int)(d->height * DISP_H);

    draw_box(bx, by, bw, bh, r, g, b);

    for (int k = 0; k < 6; k++) {
        int kx = (int)(d->keypoints[k][0] * DISP_W);
        int ky = (int)(d->keypoints[k][1] * DISP_H);
        draw_kp(kx, ky, r, g, b);
    }
}

static void draw_detections_on_display(ai_result_t *result)
{
	if (result->nb_detect == 0) return;
    /* 暂停 PIPE1 连续写入，防止 DCMIPP 新帧覆盖正在绘制的框 */
    HAL_DCMIPP_CSI_PIPE_Stop(&hdcmipp, DCMIPP_PIPE1, DCMIPP_VIRTUAL_CHANNEL0);
    for (uint32_t i = 0; i < result->nb_detect; i++) {
        draw_one_detection(&result->detections[i], 0, 255, 0);
    }
    HAL_DCMIPP_CSI_PIPE_Start(&hdcmipp, DCMIPP_PIPE1, DCMIPP_VIRTUAL_CHANNEL0,
    (uint32_t)g_ltdc_framebuf, DCMIPP_MODE_CONTINUOUS);
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
    REID_IDENTIFY,
} reid_action_t;

reid_action_t reid_pending;
static char          reid_name[FACE_NAME_MAX];
int           g_capture_pending = 0; /* set by 'register' or 's' cmd, consumed by pipeline */

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
static int          g_auto_dump    = 0;  /* 1 = hex dump face crop on every identify */

static void send_face_dump(void);

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
        g_capture_pending = 1;  /* first capture at current angle */
        printf("[REID] Enrolling '%s' — send 's' for next sample\r\n", reid_name);
    }
    else if (strcmp(cmd, "s") == 0) {
        if (reid_pending == REID_REGISTER) {
            g_capture_pending = 1;
        } else {
            printf("[REID] Not enrolling — use 'register <name>' first\r\n");
        }
    }
    else if (strcmp(cmd, "identify") == 0) {
        reid_pending = REID_IDENTIFY;
        if (g_has_selection) {
            printf("[REID] Will verify as '%s' on next frame\r\n", g_selected_entry.name);
        } else {
            printf("[REID] Will identify face on next frame (no selection)\r\n");
        }
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
    else if (strcmp(cmd, "saveface") == 0) {
        storage_save_faces();
    }
    else if (strcmp(cmd, "loadface") == 0) {
        storage_load_faces();
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
        printf("  crccc             -- CRC32 check of network_fc weights\r\n");
        printf("  dumpface          -- hex dump 112x112 face crop for PC comparison\r\n");
        printf("  --- Camera ---\r\n");
        printf("  cam status        -- print gain/exposure/AEC/AWB\r\n");
        printf("  cam auto 0|1      -- 0=manual AEC+AWB off, 1=auto AEC+AWB on\r\n");
        printf("  cam gain <0-72000>  -- set sensor gain (mdB), disables AEC\r\n");
        printf("  cam exp <0-33266>   -- set exposure (us), disables AEC\r\n");
        printf("  cam wb <K>         -- set WB: 0=auto, 2856=A(warm), 4000=TL84, 5000=D50, 6500=D65(cool)\r\n");
    }
    else if (strcmp(cmd, "crccc") == 0) {
        extern uint32_t fc_weight_crc32(void);
        uint32_t crc = fc_weight_crc32();
        printf("[CRC-FC] 0x71200000 + 1156289 bytes -> CRC32=0x%08lX (expect 0xF3588EFA)\r\n",
               (unsigned long)crc);
    }
    else if (strcmp(cmd, "dumpface") == 0) {
        send_face_dump();
    }
    else if (strcmp(cmd, "dumpface on") == 0) {
        g_auto_dump = 1;
        printf("[DUMP] Auto-dump ON — face crop sent on every identify\r\n");
    }
    else if (strcmp(cmd, "dumpface off") == 0) {
        g_auto_dump = 0;
        printf("[DUMP] Auto-dump OFF\r\n");
    }
    /* ---- Camera manual control ---- */
    else if (strcmp(cmd, "cam status") == 0) {
        imx335_print_status();
    }
    else if (strncmp(cmd, "cam gain ", 9) == 0) {
        uint32_t val = (uint32_t)atoi(cmd + 9);
        imx335_set_manual_gain(val);
    }
    else if (strncmp(cmd, "cam exp ", 8) == 0) {
        uint32_t val = (uint32_t)atoi(cmd + 8);
        imx335_set_manual_exposure(val);
    }
    else if (strncmp(cmd, "cam wb ", 7) == 0) {
        uint32_t ct = (uint32_t)atoi(cmd + 7);
        imx335_set_wb_mode(ct);
    }
    else if (strncmp(cmd, "cam auto ", 9) == 0) {
        uint8_t en = (uint8_t)atoi(cmd + 9);
        imx335_set_auto_mode(en);
    }
}

/* -------------------------------------------------------------------------- */
/*                     ReID Pipeline (triggered by serial cmd)                 */
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

static void run_reid_pipeline(ai_result_t *result)
{
    if (reid_pending == REID_IDLE) return;
    if (result->nb_detect == 0) {
        /* In multi-enroll mode keep waiting for a face; otherwise skip. */
        if (reid_pending != REID_REGISTER)
            reid_pending = REID_IDLE;
        return;
    }
    /* Waiting for 's' trigger — don't run inference until then */
    if (reid_pending == REID_REGISTER && !g_capture_pending)
        return;

    /* Use the highest-confidence detection */
    ai_detection_t *best = &result->detections[0];

    printf("[REID] DBG1: crop start, box=(%.2f,%.2f,%.2f,%.2f)\r\n",
           best->x_center, best->y_center, best->width, best->height);

    /* Step 1: Stop PIPE1, crop from g_ltdc_framebuf, restart PIPE1.
       At this point (~6.4ms after VSYNC), PIPE1 has written ~93 lines of
       the current frame. The crop region (typically y≈140-340 for a centered
       face) falls in lines 93-377 which are clean camera data from the
       previous PIPE1 cycle. Stopping PIPE1 prevents concurrent writes during
       the CPU read. */
    HAL_DCMIPP_CSI_PIPE_Stop(&hdcmipp, DCMIPP_PIPE1, DCMIPP_VIRTUAL_CHANNEL0);
    ai_crop_resize_face_112((uint8_t *)g_ltdc_framebuf, best, FACE_CROP_BUF);
    HAL_DCMIPP_CSI_PIPE_Start(&hdcmipp, DCMIPP_PIPE1, DCMIPP_VIRTUAL_CHANNEL0,
                               (uint32_t)g_ltdc_framebuf, DCMIPP_MODE_CONTINUOUS);

    printf("[REID] DBG2: crop done, quantize start, fc_in=%p\r\n", buffer_in_fc);

    /* Step 2: uint8 → int8 quantization */
    quantize_u8_to_s8(FACE_CROP_BUF, (int8_t *)buffer_in_fc,
                      LL_ATON_NETWORK_FC_IN_1_SIZE_BYTES);

    printf("[REID] DBG3: quantize done, cache flush\r\n");

    /* Step 3: Cache maintenance for network_fc input */
    SCB_CleanInvalidateDCache_by_Addr(
        (uint32_t *)buffer_in_fc, LL_ATON_NETWORK_FC_IN_1_SIZE_BYTES);

    printf("[REID] DBG4: starting fc inference...\r\n");

    /* Step 4: Run network_fc inference */
    float embedding[FACE_EMBEDDING_DIM];
    ai_face_reid_run(embedding);

    printf("[REID] DBG5: fc inference done\r\n");

    /* DEBUG: print full 128-dim embedding */
    printf("[REID-EMB] ");
    for (int i = 0; i < FACE_EMBEDDING_DIM; i++) {
        printf("%.4f ", embedding[i]);
        if ((i + 1) % 16 == 0 && i < FACE_EMBEDDING_DIM - 1)
            printf("\r\n[REID-EMB] ");
    }
    printf("\r\n");

    /* Quick statistics to detect garbage at a glance */
    float emb_min = embedding[0], emb_max = embedding[0], emb_sum_sq = 0.0f;
    for (int i = 0; i < FACE_EMBEDDING_DIM; i++) {
        if (embedding[i] < emb_min) emb_min = embedding[i];
        if (embedding[i] > emb_max) emb_max = embedding[i];
        emb_sum_sq += embedding[i] * embedding[i];
    }
    printf("[REID-STAT] min=%.4f max=%.4f rms=%.4f l2norm=%.4f\r\n",
           emb_min, emb_max, sqrtf(emb_sum_sq / FACE_EMBEDDING_DIM), sqrtf(emb_sum_sq));

    /* Cosine similarity with previous embedding (same face should be > 0.7) */
    {
        static float prev_emb[FACE_EMBEDDING_DIM];
        static int has_prev = 0;
        if (has_prev) {
            float dot = 0.0f, na = 0.0f, nb = 0.0f;
            for (int i = 0; i < FACE_EMBEDDING_DIM; i++) {
                dot += prev_emb[i] * embedding[i];
                na += prev_emb[i] * prev_emb[i];
                nb += embedding[i] * embedding[i];
            }
            printf("[REID-COSIM] prev_vs_curr=%.6f\r\n",
                   dot / (sqrtf(na) * sqrtf(nb) + 1e-30f));
        }
        memcpy(prev_emb, embedding, sizeof(prev_emb));
        has_prev = 1;
    }

    /* Step 5: Enroll or identify */
    if (reid_pending == REID_REGISTER) {
        if (g_capture_pending) {
            g_capture_pending = 0;
            int remaining = ai_face_enroll_multi(embedding, reid_name);
            if (remaining == ENROLL_QUALITY_REJECT) {
                snprintf(g_enroll_msg, sizeof(g_enroll_msg), "Keep still...");
            }
            else if (remaining == 0) {
                reid_pending = REID_IDLE;
                storage_mark_faces_dirty();
                printf("[REID] Enrolled '%s'\r\n", reid_name);

                g_enroll_state = 3;  /* waiting for ESP32 name */
                snprintf(g_enroll_msg, sizeof(g_enroll_msg), "Waiting for a name");

                /* Notify ESP32 and wait for name from mini-program */
                {
                    char user_id[32];
                    uint32_t idx = ai_face_gallery_get_count() - 1;
                    g_pending_face_code = sys_rng_get_4digit();
                    snprintf(user_id, sizeof(user_id), "user_%03lu:%04u",
                             idx + 1, g_pending_face_code);
                    uart4_send_face_enroll(user_id);
                    printf("[FACE] Sent enroll: %s, waiting for name...\r\n", user_id);

                    g_face_name_ready = 0;
                    int timeout = 9000;  /* 9000 * 10ms = 90s */
                    while (!g_face_name_ready && timeout > 0) {
                        osDelay(10);
                        timeout--;
                    }

                    if (g_face_name_ready) {
                        printf("[FACE] Name saved by handler: '%s'\r\n",
                               g_face_name_result);
                        snprintf(g_enroll_msg, sizeof(g_enroll_msg), "%s",
                                 g_face_name_result);
                    } else {
                        printf("[FACE] Name timeout, keeping '%s'\r\n", reid_name);
                        snprintf(g_enroll_msg, sizeof(g_enroll_msg), "%s",
                                 reid_name);
                    }
                    g_enroll_state = 2;  /* done */
                }
            }
            else {
                /* Sample accepted, still enrolling */
                g_enroll_progress = FACE_ENROLL_SAMPLES - remaining;
                snprintf(g_enroll_msg, sizeof(g_enroll_msg),
                         "Enroll sample %d/%d",
                         g_enroll_progress, FACE_ENROLL_SAMPLES);
                g_capture_pending = 1;  /* auto-capture next sample */
            }
        }
    }
    else if (reid_pending == REID_IDENTIFY) {
        /* Multi-frame accumulation: average N embeddings for lower noise */
        static float  ident_accum[FACE_EMBEDDING_DIM];
        static int    ident_count;
        static float  ident_first_dist;  /* single-frame dist for debug */

        if (ident_count == 0) {
            memset(ident_accum, 0, sizeof(ident_accum));
        }

        /* Accumulate raw (pre-normalized) embedding */
        for (int i = 0; i < FACE_EMBEDDING_DIM; i++) {
            ident_accum[i] += embedding[i];
        }
        ident_count++;

        /* Show single-frame stats for the first frame */
        if (ident_count == 1) {
            /* Quick single-frame identification for debug comparison */
            if (g_has_selection) {
                ai_face_verify(embedding, &g_selected_entry, &ident_first_dist);
                printf("[VERIFY] frame 1/%d single-dist=%.3f\r\n",
                       IDENTIFY_FRAMES, ident_first_dist);
            } else {
                int idx = ai_face_identify(embedding, g_identify_name, &ident_first_dist);
                if (idx >= 0)
                    printf("[REID] frame 1/%d single-dist=%.3f -> '%s'\r\n",
                           IDENTIFY_FRAMES, ident_first_dist, g_identify_name);
                else
                    printf("[REID] frame 1/%d single-dist=%.3f -> no match\r\n",
                           IDENTIFY_FRAMES, ident_first_dist);
            }
        }

        if (ident_count < IDENTIFY_FRAMES) {
            /* Keep accumulating — don't reset reid_pending yet */
            printf("[IDENT] accumulating %d/%d\r\n", ident_count, IDENTIFY_FRAMES);
        } else {
            /* All frames collected: average and L2-normalize */
            float avg[FACE_EMBEDDING_DIM];
            for (int i = 0; i < FACE_EMBEDDING_DIM; i++) {
                avg[i] = ident_accum[i] / (float)IDENTIFY_FRAMES;
            }
            /* L2-normalize the averaged embedding */
            float sum_sq = 0.0f;
            for (int i = 0; i < FACE_EMBEDDING_DIM; i++) sum_sq += avg[i] * avg[i];
            if (sum_sq > 1e-30f) {
                float inv = 1.0f / sqrtf(sum_sq);
                for (int i = 0; i < FACE_EMBEDDING_DIM; i++) avg[i] *= inv;
            }

            if (g_has_selection) {
                /* 1:1 verification with averaged embedding */
                int ok = ai_face_verify(avg, &g_selected_entry, &g_identify_dist);
                if (ok) {
                    g_identify_match = 1;
                    g_identify_index = ai_face_gallery_find_index(g_selected_entry.name);
                    strncpy(g_identify_name, g_selected_entry.name, FACE_NAME_MAX);
                    printf("[VERIFY] Match: '%s' (avg-dist=%.3f, single-dist=%.3f)\r\n",
                           g_identify_name, g_identify_dist, ident_first_dist);
                } else {
                    g_identify_match = 0;
                    printf("[VERIFY] Rejected (avg-dist=%.3f >= %.2f, single-dist=%.3f)\r\n",
                           g_identify_dist, (double)FACE_VERIFY_THRESHOLD, ident_first_dist);
                }
            } else {
                /* 1:N identification with averaged embedding */
                int idx = ai_face_identify(avg, g_identify_name, &g_identify_dist);
                if (idx >= 0) {
                    g_identify_match = 1;
                    g_identify_index = idx;
                    printf("[REID] Matched: '%s' (avg-dist=%.3f, single-dist=%.3f)\r\n",
                           g_identify_name, g_identify_dist, ident_first_dist);
                } else {
                    g_identify_match = 0;
                    printf("[REID] No match (avg-dist=%.3f, single-dist=%.3f)\r\n",
                           g_identify_dist, ident_first_dist);
                }
            }
            g_identify_done = 1;
            ident_count = 0;

            if (g_auto_dump) send_face_dump();
            reid_pending = REID_IDLE;
        }
    }
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

    for (;;) {
        if(osSemaphoreAcquire(cam_frame_sem, HAL_MAX_DELAY) != osOK)
            while(1);

        /* Cache maintenance: DCMIPP wrote to nn_input_u8 via DMA */
        SCB_CleanInvalidateDCache_by_Addr(
            (uint32_t *)nn_input_u8, sizeof(nn_input_u8));

        /* Software uint8→float32 normalization: nn_input_u8 → buffer_in */
        float *fin = (float *)buffer_in;
        for (uint32_t i = 0; i < 128 * 128 * 3; i++) {
            fin[i] = (float)nn_input_u8[i] / 255.0f;
        }

        /* Cache maintenance: CPU wrote to buffer_in */
        SCB_CleanInvalidateDCache_by_Addr(
            (uint32_t *)buffer_in, LL_ATON_NETWORK_F_IN_1_SIZE_BYTES);

        /* Run NPU inference + post-processing */
        memset(&result, 0, sizeof(result));
        MX_X_CUBE_AI_Process_User(&result);

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
        process_serial_commands();

        /* Run ReID pipeline if triggered — stops PIPE1 during CPU crop
           to read clean camera data, then restarts for draw_detections. */
        run_reid_pipeline(&result);

        /* Draw detection boxes + keypoints on LTDC layer 2 overlay */
        draw_detections_on_display(&result);

        if(g_trigger_identify == 1)
        {
        	g_trigger_identify = 0;
        	reid_pending = REID_IDENTIFY;
        }
        HAL_GPIO_WritePin(GPIOG,GPIO_PIN_10,0);
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

void ai_select_face_by_index(int idx) {
    face_entry_t entries[FACE_GALLERY_MAX];
    uint32_t count = FACE_GALLERY_MAX;
    ai_face_gallery_export(entries, &count);
    if (idx >= 0 && idx < (int)count) {
        if (ai_face_select(entries[idx].name, &g_selected_entry) == 0) {
            g_has_selection = 1;
            printf("[REID] Dropdown selected '%s' for 1:1 verify\r\n", entries[idx].name);
        }
    }
}

void ai_refresh_dropdown_users(void) {
    face_entry_t entries[FACE_GALLERY_MAX];
    uint32_t count = FACE_GALLERY_MAX;
    ai_face_gallery_export(entries, &count);

    static char options[2048];
    strcpy(options, "   Choose ID");
    for (uint32_t i = 0; i < count; i++) {
        strcat(options, "\n");
        strcat(options, entries[i].name);
    }
    lv_dropdown_set_options(objects.dropdown_users, options);
    lv_dropdown_set_selected(objects.dropdown_users, 0);
}
