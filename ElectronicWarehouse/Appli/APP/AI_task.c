#include "AI_task.h"
#include "main.h"
#include "stai.h"
#include "stai_network.h"
#include "app_postprocess.h"
#include "app_config.h"
#include "fd_blazeface_pp_if.h"
#include "fd_pp_output_if.h"
#include "fd_blazeface_anchors_0.h"
#include "fd_blazeface_anchors_1.h"
#include "dcmipp.h"
#include "rgblcd.h"
#include "npu_cache.h"
#include "FreeRTOS.h"
#include "task.h"
/* ========================================================================
 * 静态配置
 * ======================================================================== */

/* LCD 可绘制区域 — 按你的 LTDC 配置调整 */
#define DISP_WIDTH   rgblcddev.width
#define DISP_HEIGHT  rgblcddev.height

/* 前景刷新使用的颜色索引（循环使用） */
#define NUMBER_COLORS  8
static const uint32_t colors[NUMBER_COLORS] = {
    RED, GREEN, BLUE, MAGENTA, CYAN, YELLOW, BROWN, LIGHTBLUE
};

static const int CIRCLE_RADIUS = 3;

/* ========================================================================
 * NN 上下文 & 缓冲
 * ======================================================================== */

/* 网络上下文 — 32 字节对齐，放在 HyperRAM (.EXTRAM) */
static uint8_t network_context_buf[STAI_NETWORK_CONTEXT_SIZE]
    __attribute__((section(".EXTRAM"), aligned(32)));
#define network_context ((stai_ptr)network_context_buf)

/* NN 输出 buffer 指针 — 由 stai_network_get_outputs 填充 */
static stai_ptr nn_out[STAI_NETWORK_OUT_NUM];
static int32_t  nn_out_len[STAI_NETWORK_OUT_NUM];

/* NN 输入 buffer — 128×128×3 = 49152 bytes */
static uint8_t nn_in[STAI_NETWORK_IN_1_SIZE_BYTES]
    __attribute__((section(".noncacheable"), aligned(32)));

/* ========================================================================
 * 后处理状态
 * ======================================================================== */

static fd_blazeface_pp_static_param_t pp_params;

/* 所有检测候选的输出 buffer（最多 896 = 512+384） */
#define AI_TOTAL_BOXES  (AI_FD_BLAZEFACE_PP_OUT_0_NB_BOXES + AI_FD_BLAZEFACE_PP_OUT_1_NB_BOXES)
static fd_pp_outBuffer_t out_detections[AI_TOTAL_BOXES];
static fd_pp_keyPoints_t  out_keyPoints[AI_TOTAL_BOXES * AI_FD_BLAZEFACE_PP_NB_KEYPOINTS];
static fd_pp_out_t pp_output;

/* 任务句柄 — 用于 ISR 通知 */
osThreadId_t AITaskHandle;
const osThreadAttr_t AITask_attributes = {
    .name      = "AITask",
    .priority  = (osPriority_t)osPriorityBelowNormal,
    .stack_size = 1024 * 4
};



extern osSemaphoreId_t cam_frame_sem;
/* ========================================================================
 * 前向声明
 * ======================================================================== */

static void NPU_Init(void);
static int  NN_Init(void);
static int  PP_Init(void);
static int  CaptureSnapshot(void);
static void DrawResults(void);
static void DrawFace(fd_pp_outBuffer_t *detect);
static void DrawKeypoint(fd_pp_keyPoints_t *key, uint32_t color);
static void ConvertPoint(float32_t xi, float32_t yi, int *xo, int *yo);
static int  ClampPoint(int *x, int *y);

/* ========================================================================
 * 任务入口
 * ======================================================================== */

void StartAITask(void *argument)
{
    (void)argument;

    /* ---- 一次性初始化 ---- */
    NPU_Init();

    if (NN_Init() != 0) {
        Error_Handler();
    }

    if (PP_Init() != 0) {
        Error_Handler();
    }
    /* ---- 主循环 ---- */
    for (;;) {
        /* 1. DCMIPP PIPE2 快照 128×128 → nn_in（内部等待 FrameEventCallback） */
        if (CaptureSnapshot() != 0) {
            continue;
        }

        /* 2. D-Cache 无效化 */
        SCB_InvalidateDCache_by_Addr(nn_in, sizeof(nn_in));

        /* 3. NPU 推理 */
        int ret = stai_network_run(network_context, STAI_MODE_SYNC);
        if (ret != STAI_SUCCESS) {
            continue;
        }

        /* 4. 后处理 */
        pp_params.nb_detect = 0;
        ret = app_postprocess_run((void **)nn_out, STAI_NETWORK_OUT_NUM,
                                  &pp_output, &pp_params);
        if (ret != 0) {
            continue;
        }

        /* 5. 丢弃 NN 输出区域的 D-Cache */
        for (int i = 0; i < STAI_NETWORK_OUT_NUM; i++) {
            SCB_InvalidateDCache_by_Addr(nn_out[i], nn_out_len[i]);
        }

        /* 6. 绘制检测结果到 LCD 前景层 */
        DrawResults();
    }
}

/* ========================================================================
 * NPU 初始化
 * ======================================================================== */

static void NPU_Init(void)
{
    /* 使能 NPU 时钟并复位 */
    __HAL_RCC_NPU_CLK_ENABLE();
    __HAL_RCC_NPU_FORCE_RESET();
    __HAL_RCC_NPU_RELEASE_RESET();

    /* 使能 NPU SRAM (AXISRAM3-6, 每块 448KB) */
    __HAL_RCC_AXISRAM3_MEM_CLK_ENABLE();
    __HAL_RCC_AXISRAM4_MEM_CLK_ENABLE();
    __HAL_RCC_AXISRAM5_MEM_CLK_ENABLE();
    __HAL_RCC_AXISRAM6_MEM_CLK_ENABLE();
    __HAL_RCC_RAMCFG_CLK_ENABLE();

    RAMCFG_HandleTypeDef hramcfg = {0};
    hramcfg.Instance = RAMCFG_SRAM3_AXI;
    HAL_RAMCFG_EnableAXISRAM(&hramcfg);
    hramcfg.Instance = RAMCFG_SRAM4_AXI;
    HAL_RAMCFG_EnableAXISRAM(&hramcfg);
    hramcfg.Instance = RAMCFG_SRAM5_AXI;
    HAL_RAMCFG_EnableAXISRAM(&hramcfg);
    hramcfg.Instance = RAMCFG_SRAM6_AXI;
    HAL_RAMCFG_EnableAXISRAM(&hramcfg);

    /* RIF 安全配置 — NPU 的 Master 权限 */
    __HAL_RCC_RIFSC_CLK_ENABLE();
    RIMC_MasterConfig_t RIMC_master = {0};
    RIMC_master.MasterCID = RIF_CID_1;
    RIMC_master.SecPriv = RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_PRIV;
    HAL_RIF_RIMC_ConfigMasterAttributes(RIF_MASTER_INDEX_NPU, &RIMC_master);

    /* 使能 NPU 指令/数据 Cache */
    npu_cache_enable();
}

/* ========================================================================
 * NN 初始化 — 对标参考项目 NeuralNetwork_init()
 * ======================================================================== */

static int NN_Init(void)
{
    stai_network_info info;
    int ret;

    /* ST.AI 运行时初始化 */
    ret = stai_runtime_init();
    if (ret != STAI_SUCCESS) return -1;

    /* 模型实例初始化 */
    ret = stai_network_init(network_context);
    if (ret != STAI_SUCCESS) return -1;

    /* 获取模型信息 */
    ret = stai_network_get_info(network_context, &info);
    if (ret != STAI_SUCCESS) return -1;

    /* 校验输入数 */
    if (info.n_inputs != 1) return -1;

    /* 获取输入 buffer 指针 — 指向我们预分配的 nn_in[] */
    stai_ptr nn_in_ptr = nn_in;
    ret = stai_network_set_inputs(network_context, &nn_in_ptr, info.n_inputs);
    if (ret != STAI_SUCCESS) return -1;

    /* 获取输出 buffer 指针 — 由 runtime 内部管理 */
    stai_size n_outputs = STAI_NETWORK_OUT_NUM;
    ret = stai_network_get_outputs(network_context, nn_out, &n_outputs);
    if (ret != STAI_SUCCESS) return -1;

    for (stai_size i = 0; i < n_outputs; i++) {
        nn_out_len[i] = info.outputs[i].size_bytes;
    }

    return 0;
}

/* ========================================================================
 * 后处理初始化 — 对标参考项目 app_postprocess_init()
 * ======================================================================== */

static int PP_Init(void)
{
    /* 关联每个检测框的 keypoints 数组 */
    for (int i = 0; i < AI_TOTAL_BOXES; i++) {
        out_detections[i].pKeyPoints = &out_keyPoints[i * AI_FD_BLAZEFACE_PP_NB_KEYPOINTS];
    }

    pp_output.pOutBuff  = out_detections;
    pp_output.nb_detect = 0;

    /* 获取模型量化信息 */
    stai_network_info info;
    int ret = stai_network_get_info(network_context, &info);
    if (ret != STAI_SUCCESS) return -1;

    /* 调用包装器初始化 */
    ret = app_postprocess_init(&pp_params, &info);
    if (ret != 0) return -1;

    return 0;
}

/* ========================================================================
 * DCMIPP PIPE2 快照 — 专用 AI 管道，不打断 PIPE1 显示流
 *
 *   PIPE1: 连续模式 → LCD Layer 0（摄像头画面，不中断）
 *   PIPE2: SNAPSHOT  → nn_in（128×128 RGB，逐帧触发）
 *
 *   帧完成信号由 HAL_DCMIPP_PIPE_FrameEventCallback 中释放 cam_frame_sem
 * ======================================================================== */

static int CaptureSnapshot(void)
{
    if (HAL_DCMIPP_CSI_PIPE_Start(&hdcmipp,
                                  DCMIPP_PIPE2,
                                  DCMIPP_VIRTUAL_CHANNEL0,
                                  (uint32_t)nn_in,
                                  DCMIPP_MODE_SNAPSHOT) != HAL_OK) {
        return -1;
    }

    /* 阻塞等待帧完成 — ISR 中 osSemaphoreRelease(cam_frame_sem) 唤醒 */
    if (osSemaphoreAcquire(cam_frame_sem, pdMS_TO_TICKS(100)) != osOK) {
        HAL_DCMIPP_CSI_PIPE_Stop(&hdcmipp, DCMIPP_PIPE2, DCMIPP_VIRTUAL_CHANNEL0);
        return -1;
    }

    return 0;
}

/* ========================================================================
 * 绘制 — 归一化坐标 → 屏幕像素
 * ======================================================================== */

static void ConvertPoint(float32_t xi, float32_t yi, int *xo, int *yo)
{
    *xo = (int)(DISP_WIDTH  * xi);
    *yo = (int)(DISP_HEIGHT * yi);
}

static int ClampPoint(int *x, int *y)
{
    int clamped = 0;
    if (*x < 0)          { *x = 0; clamped = 1; }
    if (*x >= DISP_WIDTH) { *x = DISP_WIDTH - 1; clamped = 1; }
    if (*y < 0)           { *y = 0; clamped = 1; }
    if (*y >= DISP_HEIGHT){ *y = DISP_HEIGHT - 1; clamped = 1; }
    return clamped;
}

static void DrawKeypoint(fd_pp_keyPoints_t *key, uint32_t color)
{
    int x, y;
    ConvertPoint(key->x, key->y, &x, &y);
    if (ClampPoint(&x, &y)) return;

    rgblcd_fill_circle((uint16_t)x, (uint16_t)y, CIRCLE_RADIUS, (uint16_t)color);
}

static void DrawFace(fd_pp_outBuffer_t *detect)
{
    int cx, cy, w, h;
    int x0, y0, x1, y1;

    ConvertPoint(detect->x_center, detect->y_center, &cx, &cy);

    /* 将归一化宽高转为像素 */
    w = (int)(DISP_WIDTH  * detect->width);
    h = (int)(DISP_HEIGHT * detect->height);

    x0 = cx - (w + 1) / 2;
    y0 = cy - (h + 1) / 2;
    x1 = cx + (w + 1) / 2;
    y1 = cy + (h + 1) / 2;

    if (ClampPoint(&x0, &y0) && ClampPoint(&x1, &y1)) return;

    /* 画边界框 */
    uint16_t color = (uint16_t)colors[detect->class_index % NUMBER_COLORS];
    rgblcd_draw_rectangle((uint16_t)x0, (uint16_t)y0,
                          (uint16_t)x1, (uint16_t)y1, color);

    /* 画 6 个面部关键点 */
    for (int i = 0; i < AI_FD_BLAZEFACE_PP_NB_KEYPOINTS; i++) {
        DrawKeypoint(&detect->pKeyPoints[i], YELLOW);
    }
}

static void DrawResults(void)
{
    for (int32_t i = 0; i < pp_output.nb_detect; i++) {
        DrawFace(&pp_output.pOutBuff[i]);
    }
}
