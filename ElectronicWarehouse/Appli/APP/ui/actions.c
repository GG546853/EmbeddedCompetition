#include "actions.h"
#include "eez-flow.h"
#include "cmsis_os.h"

osMutexId_t dma2d_mutex;
const osMutexAttr_t dma2d_mutex_attr = { .name = "dma2d_mutex" };
