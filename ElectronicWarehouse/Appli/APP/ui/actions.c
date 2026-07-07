#include "actions.h"
#include "eez-flow.h"
#include "cmsis_os.h"

osMutexId_t dma2d_mutex;
const osMutexAttr_t dma2d_mutex_attr = { .name = "dma2d_mutex" };



void action_printer(lv_event_t * e) {
}

void action_action_stop_camera(lv_event_t * e) {
}

void action_action_recognize(lv_event_t * e) {
}

void action_iobutton(lv_event_t * e) {
}
