#include "lvgl.h"
#include "lv_port_indev.h"
#include "touch.h"     // tp_dev
#include "rgblcd.h"    // rgblcddev.width/height
#include "cmsis_os.h"

extern osMutexId_t pd_i2c_mutex;

static void touchpad_read(lv_indev_t *indev, lv_indev_data_t *data)
{
	if (pd_i2c_mutex != NULL) {
		osMutexAcquire(pd_i2c_mutex, osWaitForever);
	}
	tp_dev.scan(0);
	if (pd_i2c_mutex != NULL) {
		osMutexRelease(pd_i2c_mutex);
	}
	if (tp_dev.sta & (1 << 15)) {   // 检测到触摸按下
		data->point.x = tp_dev.x[0];
		data->point.y = tp_dev.y[0];
		data->state = LV_INDEV_STATE_PRESSED;
	} else {
		data->state = LV_INDEV_STATE_RELEASED;
	}
}

void lv_port_indev_init(void) {
	tp_init();  // 自动检测触摸控制器
	lv_indev_t *indev = lv_indev_create();
	lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
	lv_indev_set_read_cb(indev, touchpad_read);
}
