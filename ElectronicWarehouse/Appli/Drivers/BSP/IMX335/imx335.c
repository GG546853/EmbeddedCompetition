/**
 ****************************************************************************************************
 * @file        imx335.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2025-01-13
 * @brief       IMX335驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 * 
 * 实验平台:正点原子 N647开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 * 
 ****************************************************************************************************
 */

#include "imx335.h"
#include "STM32_IMX335/imx335.h"
#include "isp_api.h"
#include "isp_param_conf.h"
#include "cmsis_os.h"
#include "sys.h"

extern DCMIPP_HandleTypeDef hdcmipp;
extern osSemaphoreId_t cam_frame_sem;
static __IO uint32_t imx335_capture_frame_count = 0;
static IMX335_Object_t imx335_object = {0};
static ISP_HandleTypeDef imx335_hisp = {0};
static int32_t imx335_isp_gain;
static int32_t imx335_isp_exposure;

static uint8_t imx335_dcmipp_init(void);
static int32_t imx335_io_init(void);
int32_t imx335_io_deinit(void);
static int32_t imx335_io_writereg(uint16_t dev_addr, uint16_t reg, uint8_t *data, uint16_t length);
static int32_t imx335_io_readreg(uint16_t dev_addr, uint16_t reg, uint8_t *data, uint16_t length);
static int32_t imx335_io_gettick(void);
static ISP_StatusTypeDef imx335_get_sensor_info_helper(uint32_t instance, ISP_SensorInfoTypeDef *sensor_info);
static ISP_StatusTypeDef imx335_set_sensor_gain_helper(uint32_t instance, int32_t gain);
static ISP_StatusTypeDef imx335_get_sensor_gain_helper(uint32_t instance, int32_t *gain);
static ISP_StatusTypeDef imx335_set_sensor_exposure_helper(uint32_t instance, int32_t exposure);
static ISP_StatusTypeDef imx335_get_sensor_exposure_helper(uint32_t instance, int32_t *exposure);

/**
 * @brief   初始化IMX335
 * @param   无
 * @retval  初始化结果
 * @arg     0: 初始化成功
 * @arg     1: 初始化失败
 */
uint8_t imx335_init(void)
{
    IMX335_IO_t imx335_io_struct = {0};
    uint32_t id;
    ISP_AppliHelpersTypeDef isp_appli_helpers_struct = {0};
    ISP_StatAreaTypeDef isp_stat_area_struct = {0};

    if (imx335_dcmipp_init() != 0)
    {
    	printf("[IMX335] dcmipp_init FAILED\r\n");
        return 1;
    }
    printf("[IMX335] dcmipp_init OK\r\n");

    imx335_io_struct.Init = imx335_io_init;
    imx335_io_struct.DeInit = imx335_io_deinit;
    imx335_io_struct.Address = IMX335_ADDR;
    imx335_io_struct.WriteReg = imx335_io_writereg;
    imx335_io_struct.ReadReg = imx335_io_readreg;
    imx335_io_struct.GetTick = imx335_io_gettick;
    if (IMX335_RegisterBusIO(&imx335_object, &imx335_io_struct) != IMX335_OK)
    {
        return 1;
    }
    else if (IMX335_ReadID(&imx335_object, &id) != IMX335_OK)
    {
    	printf("[IMX335] ReadID FAILED\r\n");
        return 1;
    }
    else
    {
        if (id != (uint32_t)IMX335_ID)
        {
        	 printf("[IMX335] ID mismatch: got 0x%04lX, expected 0x%04X\r\n", id, IMX335_ID);
            return 1;
        }
        else
        {
            if (IMX335_Init(&imx335_object, IMX335_R2592_1944, IMX335_RAW_RGGB10) != IMX335_OK)
            {
            	printf("1\r\n");
                return 1;
            }
            else if (IMX335_SetFrequency(&imx335_object, IMX335_INCK_24MHZ) != IMX335_OK)
            {
            	printf("2\r\n");
                return 1;
            }
        }
    }

    isp_appli_helpers_struct.GetSensorInfo = imx335_get_sensor_info_helper;
    isp_appli_helpers_struct.SetSensorGain = imx335_set_sensor_gain_helper;
    isp_appli_helpers_struct.GetSensorGain = imx335_get_sensor_gain_helper;
    isp_appli_helpers_struct.SetSensorExposure = imx335_set_sensor_exposure_helper;
    isp_appli_helpers_struct.GetSensorExposure = imx335_get_sensor_exposure_helper;
    isp_stat_area_struct.X0 = 0;
    isp_stat_area_struct.Y0 = 0;
    isp_stat_area_struct.XSize = 2592;
    isp_stat_area_struct.YSize = 1944;
    ISP_Init(&imx335_hisp, &hdcmipp, 0, &isp_appli_helpers_struct, &isp_stat_area_struct, ISP_IQParamCacheInit[0]);

    if (ISP_Start(&imx335_hisp) != ISP_OK)
    {
    	printf("3\r\n");
        return 1;
    }

    return 0;
}

/**
 * @brief   IMX335采集图像
 * @param   无
 * @retval  采集结果
 * @arg     0: 采集成功
 * @arg     1: 采集失败
 */
uint8_t imx335_get_capture(uint32_t address)
{
    imx335_capture_frame_count = 0;
    if (HAL_DCMIPP_CSI_PIPE_Start(&hdcmipp, DCMIPP_PIPE1, DCMIPP_VIRTUAL_CHANNEL0, address, DCMIPP_MODE_SNAPSHOT) != HAL_OK)
    {
        return 1;
    }

    return 0;
}

/**
 * @brief   启动IMX335采集图像
 * @param   无
 * @retval  启动结果
 * @arg     0: 启动成功
 * @arg     1: 启动失败
 */
extern uint8_t nn_input_u8[128 * 128 * 3];
uint8_t imx335_start_capture(uint32_t address)
{
    imx335_capture_frame_count = 0;
    if (HAL_DCMIPP_CSI_PIPE_Start(&hdcmipp, DCMIPP_PIPE1, DCMIPP_VIRTUAL_CHANNEL0, address, DCMIPP_MODE_CONTINUOUS) != HAL_OK)
    {
        printf("[IMX335] CSI_PIPE_Start PIPE1 FAILED\r\n");
        return 1;
    }
    if (HAL_DCMIPP_CSI_PIPE_Start(&hdcmipp, DCMIPP_PIPE2, DCMIPP_VIRTUAL_CHANNEL0, (uint32_t)nn_input_u8, DCMIPP_MODE_CONTINUOUS) != HAL_OK)
    {
        printf("[IMX335] CSI_PIPE_Start PIPE2 FAILED\r\n");
        return 1;
    }
    return 0;
}

/**
 * @brief   停止IMX335采集图像
 * @param   无
 * @retval  无
 */
void imx335_stop_capture(void)
{
    HAL_DCMIPP_CSI_PIPE_Stop(&hdcmipp, DCMIPP_PIPE1, DCMIPP_VIRTUAL_CHANNEL0);
    HAL_DCMIPP_CSI_PIPE_Stop(&hdcmipp, DCMIPP_PIPE2, DCMIPP_VIRTUAL_CHANNEL0);
}



/**
 * @brief   获取IMX335采集帧数
 * @param   无
 * @retval  帧数
 */
uint32_t imx335_get_capture_frame_count(void)
{
    return imx335_capture_frame_count;
}

/**
 * @brief   IMX335 ISP后台处理函数
 * @param   无
 * @retval  处理结果
 * @arg     0: 处理成功
 * @arg     1: 处理失败
 */
uint8_t imx335_isp_background_process(void)
{
    if (ISP_BackgroundProcess(&imx335_hisp) != ISP_OK)
    {
        return 1;
    }

    return 0;
}

/**
 * @brief   IMX335 DCMIPP pipe 帧事件回调函数
 * @param   pHdcmipp: DCMIPP句柄指针
 * @param   pipe: DCMIPP pipe号
 * @retval  无
 */
void imx335_dcmipp_pipe_frame_cb(DCMIPP_HandleTypeDef *hdcmipp, uint32_t pipe)
{
    UNUSED(hdcmipp);

    imx335_capture_frame_count++;
    if (pipe == DCMIPP_PIPE1) {
        osSemaphoreRelease(cam_frame_sem);
    }
}

/**
 * @brief   IMX335 DCMIPP pipe Vsync事件回调函数
 * @param   pHdcmipp: DCMIPP句柄指针
 * @param   pipe: DCMIPP pipe号
 * @retval  无
 */
void imx335_dcmipp_pipe_vsync_cb(DCMIPP_HandleTypeDef *hdcmipp, uint32_t pipe)
{
    UNUSED(hdcmipp);

    switch (pipe)
    {
        case DCMIPP_PIPE0:
        {
            ISP_IncDumpFrameId(&imx335_hisp);
            break;
        }
        case DCMIPP_PIPE1:
        {
            ISP_IncMainFrameId(&imx335_hisp);
            ISP_GatherStatistics(&imx335_hisp);
            break;
        }
        case DCMIPP_PIPE2:
        {
            ISP_IncAncillaryFrameId(&imx335_hisp);
            break;
        }
    }
}

/**
 * @brief   初始化IMX335 DCMIPP
 * @param   无
 * @retval  初始化结果
 * @arg     0: 初始化成功
 * @arg     1: 初始化失败
 */
static uint8_t imx335_dcmipp_init(void)
{
    DCMIPP_CSI_ConfTypeDef dcmipp_csi_conf_struct = {0};
    DCMIPP_CSI_PIPE_ConfTypeDef dcmipp_csi_pipe_conf_struct = {0};
    DCMIPP_PipeConfTypeDef dcmipp_pipe_conf_struct = {0};
    DCMIPP_DownsizeTypeDef dcmipp_down_size_struct = {0};

    IMX335_PWDN(1);
    IMX335_RST(0);
    osDelay(200);

    IMX335_RST(1);
    osDelay(50);

    hdcmipp.Instance = DCMIPP;
    if (HAL_DCMIPP_Init(&hdcmipp) != HAL_OK)
    {
        return 1;
    }

    dcmipp_csi_conf_struct.NumberOfLanes = DCMIPP_CSI_TWO_DATA_LANES;
    dcmipp_csi_conf_struct.DataLaneMapping = DCMIPP_CSI_PHYSICAL_DATA_LANES;
    dcmipp_csi_conf_struct.PHYBitrate = DCMIPP_CSI_PHY_BT_1200;
    if (HAL_DCMIPP_CSI_SetConfig(&hdcmipp, &dcmipp_csi_conf_struct) != HAL_OK)
    {
        return 1;
    }

    if (HAL_DCMIPP_CSI_SetVCConfig(&hdcmipp, DCMIPP_VIRTUAL_CHANNEL0, DCMIPP_CSI_DT_BPP10) != HAL_OK)
    {
        return 1;
    }

    dcmipp_csi_pipe_conf_struct.DataTypeMode = DCMIPP_DTMODE_DTIDA;
    dcmipp_csi_pipe_conf_struct.DataTypeIDA = DCMIPP_DT_RAW10;
    dcmipp_csi_pipe_conf_struct.DataTypeIDB = DCMIPP_DT_RAW10;
    if (HAL_DCMIPP_CSI_PIPE_SetConfig(&hdcmipp, DCMIPP_PIPE1, &dcmipp_csi_pipe_conf_struct) != HAL_OK)
    {
        return 1;
    }

    dcmipp_pipe_conf_struct.FrameRate  = DCMIPP_FRAME_RATE_ALL;
    dcmipp_pipe_conf_struct.PixelPipePitch = 2400;
    dcmipp_pipe_conf_struct.PixelPackerFormat = DCMIPP_PIXEL_PACKER_FORMAT_RGB888_YUV444_1;
    if (HAL_DCMIPP_PIPE_SetConfig(&hdcmipp, DCMIPP_PIPE1, &dcmipp_pipe_conf_struct) != HAL_OK)
    {
        return 1;
    }

    dcmipp_down_size_struct.VSize = 480;
    dcmipp_down_size_struct.HSize = 800;
    dcmipp_down_size_struct.VRatio = 33161;
    dcmipp_down_size_struct.HRatio = 25656;
    dcmipp_down_size_struct.VDivFactor = 253;
    dcmipp_down_size_struct.HDivFactor = 316;
    if (HAL_DCMIPP_PIPE_SetDownsizeConfig(&hdcmipp, DCMIPP_PIPE1, &dcmipp_down_size_struct) != HAL_OK)
    {
        return 1;
    }

    if (HAL_DCMIPP_PIPE_EnableDownsize(&hdcmipp, DCMIPP_PIPE1) != HAL_OK)
    {
        return 1;
    }



    DCMIPP_CSI_PIPE_ConfTypeDef pipe2_csi_conf = {0};
    pipe2_csi_conf.DataTypeMode = DCMIPP_DTMODE_DTIDA;
    pipe2_csi_conf.DataTypeIDA = DCMIPP_DT_RAW10;
    pipe2_csi_conf.DataTypeIDB = DCMIPP_DT_RAW10;
	if (HAL_DCMIPP_CSI_PIPE_SetConfig(&hdcmipp, DCMIPP_PIPE2, &pipe2_csi_conf) != HAL_OK)
	{
    	return 1;
	}
    DCMIPP_PipeConfTypeDef pipe2_conf = {0};
    pipe2_conf.FrameRate  = DCMIPP_FRAME_RATE_ALL;
    pipe2_conf.PixelPipePitch = 128 * 3;
    pipe2_conf.PixelPackerFormat = DCMIPP_PIXEL_PACKER_FORMAT_RGB888_YUV444_1; // NPU 喜欢 RGB 分离的格式
    if (HAL_DCMIPP_PIPE_SetConfig(&hdcmipp, DCMIPP_PIPE2, &pipe2_conf) != HAL_OK)
	{
    	return 1;
	}

    DCMIPP_DecimationConfTypeDef decConfig = {0};
    decConfig.HRatio = DCMIPP_HDEC_1_OUT_4;
    decConfig.VRatio = DCMIPP_VDEC_1_OUT_4;
    if (HAL_DCMIPP_PIPE_SetDecimationConfig(&hdcmipp, DCMIPP_PIPE2, &decConfig) != HAL_OK)
    {
        return 1;
    }
    if (HAL_DCMIPP_PIPE_EnableDecimation(&hdcmipp, DCMIPP_PIPE2) != HAL_OK)
    {
        return 1;
    }

    DCMIPP_DownsizeTypeDef pipe2_down_size = {0};
    pipe2_down_size.HSize = 128;
    pipe2_down_size.VSize = 128;

    pipe2_down_size.HDivFactor = 202;

    pipe2_down_size.VDivFactor = 269;

    pipe2_down_size.VRatio = 31284;

    pipe2_down_size.HRatio = 41734;

    if (HAL_DCMIPP_PIPE_SetDownsizeConfig(&hdcmipp, DCMIPP_PIPE2, &pipe2_down_size) != HAL_OK)
    {
        return 1;
    }

    if (HAL_DCMIPP_PIPE_EnableDownsize(&hdcmipp, DCMIPP_PIPE2) != HAL_OK)
    {
        return 1;
    }

    return 0;
}

/********************************************************************************************************/
/* 摄像头软 I2C — SCL=PD14, SDA=PC2 */

#define CAM_IIC_SCL_PORT    GPIOD
#define CAM_IIC_SCL_PIN     GPIO_PIN_14
#define CAM_IIC_SDA_PORT    GPIOC
#define CAM_IIC_SDA_PIN     GPIO_PIN_2

#define CAM_SCL_H()   HAL_GPIO_WritePin(CAM_IIC_SCL_PORT, CAM_IIC_SCL_PIN, GPIO_PIN_SET)
#define CAM_SCL_L()   HAL_GPIO_WritePin(CAM_IIC_SCL_PORT, CAM_IIC_SCL_PIN, GPIO_PIN_RESET)
#define CAM_SDA_H()   HAL_GPIO_WritePin(CAM_IIC_SDA_PORT, CAM_IIC_SDA_PIN, GPIO_PIN_SET)
#define CAM_SDA_L()   HAL_GPIO_WritePin(CAM_IIC_SDA_PORT, CAM_IIC_SDA_PIN, GPIO_PIN_RESET)
#define CAM_SDA_READ  HAL_GPIO_ReadPin(CAM_IIC_SDA_PORT, CAM_IIC_SDA_PIN)

static void cam_iic_delay(void)
{
    sys_delay_us(5);
}

/**
 * @brief   IMX335 IO初始化
 * @param   无
 * @retval  执行结果
 */
static int32_t imx335_io_init(void)
{
    GPIO_InitTypeDef gpio = {0};

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();

    /* SCL = PD14, 开漏输出 + 上拉 */
    gpio.Pin = CAM_IIC_SCL_PIN;
    gpio.Mode = GPIO_MODE_OUTPUT_OD;
    gpio.Pull = GPIO_PULLUP;
    gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(CAM_IIC_SCL_PORT, &gpio);

    /* SDA = PC2, 开漏输出 + 上拉 */
    gpio.Pin = CAM_IIC_SDA_PIN;
    HAL_GPIO_Init(CAM_IIC_SDA_PORT, &gpio);

    /* 释放总线 */
    CAM_SCL_H();
    CAM_SDA_H();
    cam_iic_delay();

    return 0;
}

/**
 * @brief   IMX335 IO反初始化 — PC2释放为高, PD14切到AF4(I2C2_SCL)给触摸屏
 * @param   无
 * @retval  执行结果
 */
int32_t imx335_io_deinit(void)
{
    GPIO_InitTypeDef gpio = {0};

    /* PC2 保持输出高，释放 SDA */
    HAL_GPIO_WritePin(CAM_IIC_SDA_PORT, CAM_IIC_SDA_PIN, GPIO_PIN_SET);

    /* PD14 切换到 AF4 给 I2C2 触摸屏用 */
    gpio.Pin = CAM_IIC_SCL_PIN;
    gpio.Mode = GPIO_MODE_AF_OD;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    gpio.Alternate = GPIO_AF4_I2C2;
    HAL_GPIO_Init(CAM_IIC_SCL_PORT, &gpio);

    return 0;
}

static void cam_iic_start(void)
{
    CAM_SDA_H();
    CAM_SCL_H();
    cam_iic_delay();
    CAM_SDA_L();
    cam_iic_delay();
    CAM_SCL_L();
    cam_iic_delay();
}

static void cam_iic_stop(void)
{
    CAM_SDA_L();
    cam_iic_delay();
    CAM_SCL_H();
    cam_iic_delay();
    CAM_SDA_H();
    cam_iic_delay();
}

static uint8_t cam_iic_wait_ack(void)
{
    uint8_t waittime = 0;
    uint8_t rack = 0;

    CAM_SDA_H();
    cam_iic_delay();
    CAM_SCL_H();
    cam_iic_delay();

    while (CAM_SDA_READ)
    {
        waittime++;
        if (waittime > 250)
        {
            cam_iic_stop();
            rack = 1;
            break;
        }
        cam_iic_delay();
    }

    CAM_SCL_L();
    cam_iic_delay();
    return rack;
}

static void cam_iic_send_byte(uint8_t data)
{
    uint8_t t;

    for (t = 0; t < 8; t++)
    {
        if (data & 0x80)
            CAM_SDA_H();
        else
            CAM_SDA_L();
        cam_iic_delay();
        CAM_SCL_H();
        cam_iic_delay();
        CAM_SCL_L();
        data <<= 1;
    }

    CAM_SDA_H();
}

static uint8_t cam_iic_read_byte(uint8_t ack)
{
    uint8_t i, receive = 0;

    for (i = 0; i < 8; i++)
    {
        receive <<= 1;
        CAM_SCL_H();
        cam_iic_delay();
        if (CAM_SDA_READ)
            receive++;
        CAM_SCL_L();
        cam_iic_delay();
    }

    if (ack)
        CAM_SDA_H();  /* NACK */
    else
        CAM_SDA_L();  /* ACK */
    cam_iic_delay();
    CAM_SCL_H();
    cam_iic_delay();
    CAM_SCL_L();
    cam_iic_delay();
    CAM_SDA_H();

    return receive;
}

/**
 * @brief   IMX335 IO写寄存器（软件I2C, SCL=PD14, SDA=PC2）
 * @param   dev_addr: 设备地址（7位地址左移1位）
 * @param   reg: 寄存器地址（16bit）
 * @param   data: 数据
 * @param   length: 数据长度
 * @retval  0成功, 1失败
 */
static int32_t imx335_io_writereg(uint16_t dev_addr, uint16_t reg, uint8_t *data, uint16_t length)
{
    static uint32_t wr_cnt = 0;
    wr_cnt++;

    cam_iic_start();
    cam_iic_send_byte(dev_addr & 0xFE);     /* 写命令 */
    if (cam_iic_wait_ack()) {
        printf("[I2C] W#%lu ACK fail at dev_addr 0x%02X\r\n", wr_cnt, dev_addr);
        cam_iic_stop(); return 1;
    }
    cam_iic_send_byte(reg >> 8);            /* 寄存器高8位 */
    if (cam_iic_wait_ack()) {
        printf("[I2C] W#%lu ACK fail at reg_hi 0x%04X\r\n", wr_cnt, reg);
        cam_iic_stop(); return 1;
    }
    cam_iic_send_byte(reg & 0xFF);          /* 寄存器低8位 */
    if (cam_iic_wait_ack()) {
        printf("[I2C] W#%lu ACK fail at reg_lo 0x%04X\r\n", wr_cnt, reg);
        cam_iic_stop(); return 1;
    }

    for (uint16_t i = 0; i < length; i++)
    {
        cam_iic_send_byte(data[i]);
        if (cam_iic_wait_ack()) {
            printf("[I2C] W#%lu ACK fail at reg 0x%04X data[%u]\r\n", wr_cnt, reg, i);
            cam_iic_stop(); return 1;
        }
    }

    cam_iic_stop();
    return 0;
}

/**
 * @brief   IMX335 IO读寄存器（软件I2C, SCL=PD14, SDA=PC2）
 * @param   dev_addr: 设备地址（7位地址左移1位）
 * @param   reg: 寄存器地址（16bit）
 * @param   data: 数据
 * @param   length: 数据长度
 * @retval  0成功, 1失败
 */
static int32_t imx335_io_readreg(uint16_t dev_addr, uint16_t reg, uint8_t *data, uint16_t length)
{
    cam_iic_start();
    cam_iic_send_byte(dev_addr & 0xFE);     /* 写命令（先写寄存器地址） */
    if (cam_iic_wait_ack()) { cam_iic_stop(); return 1; }
    cam_iic_send_byte(reg >> 8);            /* 寄存器高8位 */
    if (cam_iic_wait_ack()) { cam_iic_stop(); return 1; }
    cam_iic_send_byte(reg & 0xFF);          /* 寄存器低8位 */
    if (cam_iic_wait_ack()) { cam_iic_stop(); return 1; }

    cam_iic_start();
    cam_iic_send_byte(dev_addr | 0x01);     /* 读命令 */
    if (cam_iic_wait_ack()) { cam_iic_stop(); return 1; }

    for (uint16_t i = 0; i < length; i++)
    {
        data[i] = cam_iic_read_byte(i == (length - 1) ? 1 : 0);
    }

    cam_iic_stop();
    return 0;
}

/**
 * @brief   IMX335 IO获取Tick
 * @param   无
 * @retval  Tick
 */
static int32_t imx335_io_gettick(void)
{
    return (int32_t)HAL_GetTick();
}

/**
 * @brief   IMX335获取传感器信息
 * @param   instance: 对象
 * @param   sensor_info: 传感器信息
 * @retval  ISP状态
 */
static ISP_StatusTypeDef imx335_get_sensor_info_helper(uint32_t instance, ISP_SensorInfoTypeDef *sensor_info)
{
    UNUSED(instance);

    return (ISP_StatusTypeDef)IMX335_GetSensorInfo(&imx335_object, (IMX335_SensorInfo_t *)sensor_info);
}

/**
 * @brief   IMX335设置传感器增益
 * @param   instance: 对象
 * @param   gain: 传感器增益
 * @retval  ISP状态
 */
static ISP_StatusTypeDef imx335_set_sensor_gain_helper(uint32_t instance, int32_t gain)
{
    UNUSED(instance);

    imx335_isp_gain = gain;
    return (ISP_StatusTypeDef)IMX335_SetGain(&imx335_object, gain);
}

/**
 * @brief   IMX335获取传感器增益
 * @param   instance: 对象
 * @param   gain: 传感器增益
 * @retval  ISP状态
 */
static ISP_StatusTypeDef imx335_get_sensor_gain_helper(uint32_t instance, int32_t *gain)
{
    UNUSED(instance);

    *gain = imx335_isp_gain;
    return ISP_OK;
}

/**
 * @brief   IMX335设置传感器曝光
 * @param   instance: 对象
 * @param   exposure: 传感器曝光
 * @retval  ISP状态
 */
static ISP_StatusTypeDef imx335_set_sensor_exposure_helper(uint32_t instance, int32_t exposure)
{
    UNUSED(instance);

    imx335_isp_exposure = exposure;
    return (ISP_StatusTypeDef)IMX335_SetExposure(&imx335_object, exposure);
}

/**
 * @brief   IMX335获取传感器曝光
 * @param   instance: 对象
 * @param   exposure: 传感器曝光
 * @retval  ISP状态
 */
static ISP_StatusTypeDef imx335_get_sensor_exposure_helper(uint32_t instance, int32_t *exposure)
{
    UNUSED(instance);

    *exposure = imx335_isp_exposure;
    return ISP_OK;
}
