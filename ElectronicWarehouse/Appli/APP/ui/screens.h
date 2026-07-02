#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// Screens

enum ScreensEnum {
    _SCREEN_ID_FIRST = 1,
    SCREEN_ID_PAGE_LOCK = 1,
    SCREEN_ID_PAGE_MAIN = 2,
    SCREEN_ID_PAGE_ERROR = 3,
    _SCREEN_ID_LAST = 3
};

typedef struct _objects_t {
    lv_obj_t *page_lock;
    lv_obj_t *page_main;
    lv_obj_t *page_error;
    lv_obj_t *ring_container;
    lv_obj_t *obj0;
    lv_obj_t *obj1;
    lv_obj_t *btn_verify;
    lv_obj_t *obj2;
    lv_obj_t *ui_cont_main;
    lv_obj_t *ui_cont_sidebar;
    lv_obj_t *dashboard;
    lv_obj_t *dashboard_1;
    lv_obj_t *dashboard_2;
    lv_obj_t *ui_cont_content;
    lv_obj_t *obj3;
    lv_obj_t *tab1;
    lv_obj_t *obj4;
    lv_obj_t *obj5;
    lv_obj_t *obj6;
    lv_obj_t *obj7;
    lv_obj_t *ui_cont_cards;
    lv_obj_t *ui_cont_card1;
    lv_obj_t *ui_lbl_card1_title;
    lv_obj_t *obj8;
    lv_obj_t *ui_cont_card2;
    lv_obj_t *ui_lbl_card1_title_1;
    lv_obj_t *obj9;
    lv_obj_t *ui_cont_card3;
    lv_obj_t *ui_lbl_card1_title_2;
    lv_obj_t *obj10;
    lv_obj_t *ui_cont_card4;
    lv_obj_t *ui_lbl_card1_title_3;
    lv_obj_t *ui_bar_capacity;
    lv_obj_t *ui_cont_header;
    lv_obj_t *test3;
    lv_obj_t *ui_cont_bottom;
    lv_obj_t *ui_cont_board1;
    lv_obj_t *ui_lbl_board1_title;
    lv_obj_t *ui_cont_board1_list;
    lv_obj_t *sku;
    lv_obj_t *name;
    lv_obj_t *stock;
    lv_obj_t *ui_cont_row1;
    lv_obj_t *ui_cont_badge1;
    lv_obj_t *obj11;
    lv_obj_t *ui_cont_board2;
    lv_obj_t *ui_lbl_board1_title_1;
    lv_obj_t *ui_cont_board1_list_1;
    lv_obj_t *sku_1;
    lv_obj_t *name_1;
    lv_obj_t *obj12;
    lv_obj_t *stock_1;
    lv_obj_t *ui_cont_row2;
    lv_obj_t *obj13;
    lv_obj_t *obj14;
    lv_obj_t *test3_1;
    lv_obj_t *obj15;
    lv_obj_t *obj16;
    lv_obj_t *obj17;
    lv_obj_t *c1;
    lv_obj_t *obj18;
    lv_obj_t *obj19;
    lv_obj_t *obj20;
    lv_obj_t *obj21;
    lv_obj_t *obj22;
    lv_obj_t *obj23;
    lv_obj_t *obj24;
    lv_obj_t *obj25;
    lv_obj_t *active_border_2_;
    lv_obj_t *c1_1;
    lv_obj_t *obj26;
    lv_obj_t *obj27;
    lv_obj_t *obj28;
    lv_obj_t *obj29;
    lv_obj_t *obj30;
    lv_obj_t *obj31;
    lv_obj_t *obj32;
    lv_obj_t *obj33;
    lv_obj_t *active_border_3;
    lv_obj_t *c1_2;
    lv_obj_t *obj34;
    lv_obj_t *obj35;
    lv_obj_t *obj36;
    lv_obj_t *obj37;
    lv_obj_t *obj38;
    lv_obj_t *obj39;
    lv_obj_t *obj40;
    lv_obj_t *obj41;
    lv_obj_t *active_border_4;
    lv_obj_t *c1_3;
    lv_obj_t *obj42;
    lv_obj_t *obj43;
    lv_obj_t *obj44;
    lv_obj_t *obj45;
    lv_obj_t *obj46;
    lv_obj_t *obj47;
    lv_obj_t *obj48;
    lv_obj_t *obj49;
    lv_obj_t *active_border_5;
    lv_obj_t *obj50;
    lv_obj_t *obj51;
    lv_obj_t *obj52;
    lv_obj_t *obj53;
    lv_obj_t *obj54;
    lv_obj_t *obj55;
    lv_obj_t *obj56;
    lv_obj_t *list_material;
    lv_obj_t *obj57;
    lv_obj_t *obj58;
    lv_obj_t *obj59;
    lv_obj_t *obj60;
    lv_obj_t *obj61;
    lv_obj_t *obj62;
    lv_obj_t *obj63;
    lv_obj_t *test3_2;
    lv_obj_t *obj64;
    lv_obj_t *obj65;
    lv_obj_t *obj66;
    lv_obj_t *obj67;
    lv_obj_t *obj68;
    lv_obj_t *obj69;
    lv_obj_t *obj70;
    lv_obj_t *obj71;
    lv_obj_t *obj72;
    lv_obj_t *obj73;
    lv_obj_t *obj74;
    lv_obj_t *obj75;
    lv_obj_t *obj76;
    lv_obj_t *obj77;
    lv_obj_t *obj78;
    lv_obj_t *obj79;
    lv_obj_t *obj80;
    lv_obj_t *obj81;
} objects_t;

extern objects_t objects;

void create_screen_page_lock();
void tick_screen_page_lock();

void create_screen_page_main();
void tick_screen_page_main();

void create_screen_page_error();
void tick_screen_page_error();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/