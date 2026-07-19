#ifndef EEZ_LVGL_UI_EVENTS_H
#define EEZ_LVGL_UI_EVENTS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void action_givetime(lv_event_t * e);
extern void action_printer(lv_event_t * e);
extern void action_action_stop_camera(lv_event_t * e);
extern void action_action_recognize(lv_event_t * e);
extern void action_iobutton(lv_event_t * e);
extern void action_ver(lv_event_t * e);
extern void action_new_user(lv_event_t * e);
extern void action_save_user(lv_event_t * e);

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_EVENTS_H*/