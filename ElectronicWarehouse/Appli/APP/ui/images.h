#ifndef EEZ_LVGL_UI_IMAGES_H
#define EEZ_LVGL_UI_IMAGES_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern const lv_img_dsc_t img_p_iamge;
extern const lv_img_dsc_t img_page_wms;
extern const lv_img_dsc_t img_page_dashboard;
extern const lv_img_dsc_t img_page_inventory;
extern const lv_img_dsc_t img_page_activity;
extern const lv_img_dsc_t img_page_uimode;
extern const lv_img_dsc_t img_page_search;
extern const lv_img_dsc_t img_page_printfer;

#ifndef EXT_IMG_DESC_T
#define EXT_IMG_DESC_T
typedef struct _ext_img_desc_t {
    const char *name;
    const lv_img_dsc_t *img_dsc;
} ext_img_desc_t;
#endif

extern const ext_img_desc_t images[8];

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_IMAGES_H*/