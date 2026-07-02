#include <string.h>

#include "screens.h"
#include "images.h"
#include "fonts.h"
#include "actions.h"
#include "vars.h"
#include "styles.h"
#include "ui.h"

#include <string.h>

objects_t objects;

static const char *screen_names[] = { "page-lock", "page-main", "page-error" };
static const char *object_names[] = { "page_lock", "page_main", "page_error", "ring_container", "obj0", "obj1", "btn_verify", "obj2", "ui_cont_main", "ui_cont_sidebar", "dashboard", "dashboard_1", "dashboard_2", "ui_cont_content", "obj3", "tab1", "obj4", "obj5", "obj6", "obj7", "ui_cont_cards", "ui_cont_card1", "ui_lbl_card1_title", "obj8", "ui_cont_card2", "ui_lbl_card1_title_1", "obj9", "ui_cont_card3", "ui_lbl_card1_title_2", "obj10", "ui_cont_card4", "ui_lbl_card1_title_3", "ui_bar_capacity", "ui_cont_header", "test3", "ui_cont_bottom", "ui_cont_board1", "ui_lbl_board1_title", "ui_cont_board1_list", "sku", "name", "stock", "ui_cont_row1", "ui_cont_badge1", "obj11", "ui_cont_board2", "ui_lbl_board1_title_1", "ui_cont_board1_list_1", "sku_1", "name_1", "obj12", "stock_1", "ui_cont_row2", "obj13", "obj14", "test3_1", "obj15", "obj16", "obj17", "c1", "obj18", "obj19", "obj20", "obj21", "obj22", "obj23", "obj24", "obj25", "active_border_2_", "c1_1", "obj26", "obj27", "obj28", "obj29", "obj30", "obj31", "obj32", "obj33", "active_border_3", "c1_2", "obj34", "obj35", "obj36", "obj37", "obj38", "obj39", "obj40", "obj41", "active_border_4", "c1_3", "obj42", "obj43", "obj44", "obj45", "obj46", "obj47", "obj48", "obj49", "active_border_5", "obj50", "obj51", "obj52", "obj53", "obj54", "obj55", "obj56", "list_material", "obj57", "obj58", "obj59", "obj60", "obj61", "obj62", "obj63", "test3_2", "obj64", "obj65", "obj66", "obj67", "obj68", "obj69", "obj70", "obj71", "obj72", "obj73", "obj74", "obj75", "obj76", "obj77", "obj78", "obj79", "obj80", "obj81" };

//
// Event handlers
//

lv_obj_t *tick_value_change_obj;

static void event_handler_cb_page_lock_btn_verify(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_PRESSED) {
        e->user_data = (void *)0;
        flowPropagateValueLVGLEvent(flowState, 4, 0, e);
    }
}

static void event_handler_cb_page_main_ui_cont_main(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_SCREEN_LOADED) {
        e->user_data = (void *)0;
        flowPropagateValueLVGLEvent(flowState, -1, 3, e);
    }
}

static void event_handler_cb_page_main_dashboard(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *ta = lv_event_get_target_obj(e);
        if (tick_value_change_obj != ta) {
            bool value = lv_obj_has_state(ta, LV_STATE_CHECKED);
            assignBooleanProperty(flowState, 4, 3, value, "Failed to assign Checked state");
        }
    }
    if (event == LV_EVENT_CLICKED) {
        e->user_data = (void *)0;
        flowPropagateValueLVGLEvent(flowState, -1, 4, e);
    }
}

static void event_handler_cb_page_main_dashboard_1(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *ta = lv_event_get_target_obj(e);
        if (tick_value_change_obj != ta) {
            bool value = lv_obj_has_state(ta, LV_STATE_CHECKED);
            assignBooleanProperty(flowState, 7, 3, value, "Failed to assign Checked state");
        }
    }
    if (event == LV_EVENT_CLICKED) {
        e->user_data = (void *)1;
        flowPropagateValueLVGLEvent(flowState, -1, 5, e);
    }
}

static void event_handler_cb_page_main_dashboard_2(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *ta = lv_event_get_target_obj(e);
        if (tick_value_change_obj != ta) {
            bool value = lv_obj_has_state(ta, LV_STATE_CHECKED);
            assignBooleanProperty(flowState, 10, 3, value, "Failed to assign Checked state");
        }
    }
    if (event == LV_EVENT_CLICKED) {
        e->user_data = (void *)2;
        flowPropagateValueLVGLEvent(flowState, -1, 6, e);
    }
}

static void event_handler_cb_page_main_obj3(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *ta = lv_event_get_target_obj(e);
        if (tick_value_change_obj != ta) {
            int32_t value = lv_tabview_get_tab_active(ta);
            assignIntegerProperty(flowState, 14, 3, value, "Failed to assign Active tab in Tabview widget");
        }
    }
}

static void event_handler_cb_page_main_c1(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_CLICKED) {
        e->user_data = (void *)0;
        flowPropagateValueLVGLEvent(flowState, 77, 0, e);
    }
}

static void event_handler_cb_page_main_c1_1(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_CLICKED) {
        e->user_data = (void *)0;
        flowPropagateValueLVGLEvent(flowState, 91, 0, e);
    }
}

static void event_handler_cb_page_main_c1_2(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_CLICKED) {
        e->user_data = (void *)0;
        flowPropagateValueLVGLEvent(flowState, 105, 0, e);
    }
}

static void event_handler_cb_page_main_c1_3(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_CLICKED) {
        e->user_data = (void *)0;
        flowPropagateValueLVGLEvent(flowState, 119, 0, e);
    }
}

static void event_handler_cb_page_main_obj50(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *ta = lv_event_get_target_obj(e);
        if (tick_value_change_obj != ta) {
            int32_t value = lv_slider_get_value(ta);
            assignIntegerProperty(flowState, 134, 3, value, "Failed to assign Value in Slider widget");
        }
    }
}

static void event_handler_cb_page_main_obj51(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_PRESSED) {
        e->user_data = (void *)0;
        flowPropagateValueLVGLEvent(flowState, 135, 0, e);
    }
}

//
// Screens
//

void create_screen_page_lock() {
    void *flowState = getFlowState(0, 0);
    (void)flowState;
    lv_obj_t *obj = lv_obj_create(0);
    objects.page_lock = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 800, 480);
    lv_obj_set_style_bg_grad_stop(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x0f172a), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(obj, LV_GRAD_DIR_VER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_color(obj, lv_color_hex(0x1e293b), LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            // ring-container
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.ring_container = obj;
            lv_obj_set_pos(obj, 370, 120);
            lv_obj_set_size(obj, 120, 120);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0x06b1d4), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_color(obj, lv_color_hex(0x06a8d4), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_opa(obj, 30, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_spread(obj, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_width(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_width(obj, 120, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_height(obj, 120, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_grad_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_image_create(parent_obj);
                    objects.obj0 = obj;
                    lv_obj_set_pos(obj, 24, 24);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_image_set_src(obj, &img_p_iamge);
                    lv_image_set_scale(obj, 40);
                    lv_obj_set_style_width(obj, 65, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_height(obj, 65, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_image_recolor(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_image_recolor_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 500000000500000000, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radial_offset(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.obj1 = obj;
            lv_obj_set_pos(obj, 342, 271);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xf1f5f9), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_34, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_letter_space(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "WMS PRO");
        }
        {
            // btn-verify
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.btn_verify = obj;
            lv_obj_set_pos(obj, 380, 323);
            lv_obj_set_size(obj, 100, 50);
            lv_obj_add_event_cb(obj, event_handler_cb_page_lock_btn_verify, LV_EVENT_ALL, flowState);
            lv_obj_set_style_border_color(obj, lv_color_hex(0x06a8d4), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_side(obj, LV_BORDER_SIDE_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x06b6d4), LV_PART_MAIN | LV_STATE_HOVERED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj2 = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(0x06b6d4), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_chinese, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "开始验证");
                }
            }
        }
    }
    
    tick_screen_page_lock();
}

void tick_screen_page_lock() {
    void *flowState = getFlowState(0, 0);
    (void)flowState;
}

void create_screen_page_main() {
    void *flowState = getFlowState(0, 1);
    (void)flowState;
    lv_obj_t *obj = lv_obj_create(0);
    objects.page_main = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 800, 480);
    lv_obj_set_style_bg_color(obj, lv_color_lighten(lv_color_hex(0xf4f7f9), 64), LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            // ui_cont_main
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.ui_cont_main = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 800, LV_PCT(480));
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_add_event_cb(obj, event_handler_cb_page_main_ui_cont_main, LV_EVENT_ALL, flowState);
            lv_obj_set_style_width(obj, 800, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_height(obj, 480, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_grow(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // ui_cont_sidebar
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.ui_cont_sidebar = obj;
                    lv_obj_set_pos(obj, -933, 0);
                    lv_obj_set_size(obj, 240, LV_PCT(100));
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_width(obj, 240, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_height(obj, 480, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0x1e293b), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_COLUMN, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_column(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_image_create(parent_obj);
                            lv_obj_set_pos(obj, -241, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_image_set_src(obj, &img_page_wms);
                        }
                        {
                            // Dashboard
                            lv_obj_t *obj = lv_button_create(parent_obj);
                            objects.dashboard = obj;
                            lv_obj_set_pos(obj, -240, -335);
                            lv_obj_set_size(obj, 240, 67);
                            lv_obj_add_event_cb(obj, event_handler_cb_page_main_dashboard, LV_EVENT_ALL, flowState);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_CHECKABLE);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0x1e293b), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_border_color(obj, lv_color_hex(0x1e293b), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_border_width(obj, 100, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0x1e40af), LV_PART_MAIN | LV_STATE_CHECKED);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    lv_obj_t *obj = lv_image_create(parent_obj);
                                    lv_obj_set_pos(obj, -93, -99);
                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                    lv_image_set_src(obj, &img_page_dashboard);
                                    lv_image_set_scale(obj, 200);
                                }
                                {
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    lv_obj_set_pos(obj, 14, 6);
                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_font(obj, &ui_font_chinese18, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "仪表盘(Dashboard)");
                                }
                            }
                        }
                        {
                            // Dashboard_1
                            lv_obj_t *obj = lv_button_create(parent_obj);
                            objects.dashboard_1 = obj;
                            lv_obj_set_pos(obj, -239, -387);
                            lv_obj_set_size(obj, 240, 67);
                            lv_obj_add_event_cb(obj, event_handler_cb_page_main_dashboard_1, LV_EVENT_ALL, flowState);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_CHECKABLE);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0x1e293b), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_border_color(obj, lv_color_hex(0x1e293b), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_border_width(obj, 100, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0x1e40af), LV_PART_MAIN | LV_STATE_CHECKED);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    lv_obj_t *obj = lv_image_create(parent_obj);
                                    lv_obj_set_pos(obj, -94, -92);
                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                    lv_image_set_src(obj, &img_page_inventory);
                                    lv_image_set_scale(obj, 200);
                                }
                                {
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    lv_obj_set_pos(obj, 16, 6);
                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_font(obj, &ui_font_chinese18, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "库存管理(Inventory)");
                                }
                            }
                        }
                        {
                            // Dashboard_2
                            lv_obj_t *obj = lv_button_create(parent_obj);
                            objects.dashboard_2 = obj;
                            lv_obj_set_pos(obj, -240, -393);
                            lv_obj_set_size(obj, 240, 67);
                            lv_obj_add_event_cb(obj, event_handler_cb_page_main_dashboard_2, LV_EVENT_ALL, flowState);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_CHECKABLE);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0x1e293b), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_border_color(obj, lv_color_hex(0x1e293b), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_border_width(obj, 100, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0x1e40af), LV_PART_MAIN | LV_STATE_CHECKED);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    lv_obj_t *obj = lv_image_create(parent_obj);
                                    lv_obj_set_pos(obj, -93, -93);
                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                    lv_image_set_src(obj, &img_page_activity);
                                    lv_image_set_scale(obj, 200);
                                }
                                {
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    lv_obj_set_pos(obj, 19, 6);
                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_font(obj, &ui_font_chinese18, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "出入库记录(Actcivity)");
                                }
                            }
                        }
                    }
                }
                {
                    // ui_cont_content
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.ui_cont_content = obj;
                    lv_obj_set_pos(obj, 572, 366);
                    lv_obj_set_size(obj, LV_PCT(100), LV_PCT(100));
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_AUTO);
                    lv_obj_set_scroll_dir(obj, LV_DIR_VER);
                    lv_obj_set_style_height(obj, 480, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xf4f6f9), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_row(obj, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_width(obj, 560, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_COLUMN, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_flex_grow(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_tabview_create(parent_obj);
                            objects.obj3 = obj;
                            lv_obj_set_pos(obj, 0, -482);
                            lv_obj_set_size(obj, LV_PCT(100), LV_PCT(100));
                            lv_tabview_set_tab_bar_position(obj, LV_DIR_TOP);
                            lv_tabview_set_tab_bar_size(obj, 0);
                            lv_obj_add_event_cb(obj, event_handler_cb_page_main_obj3, LV_EVENT_ALL, flowState);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xf5f5f5), LV_PART_MAIN | LV_STATE_DEFAULT);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // Tab1
                                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Tab1");
                                    objects.tab1 = obj;
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.obj4 = obj;
                                            lv_obj_set_pos(obj, 319, 20);
                                            lv_obj_set_size(obj, 100, 25);
                                            lv_obj_set_style_text_font(obj, &ui_font_chinese18, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xf4f6f9), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "ST display:");
                                        }
                                        {
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            objects.obj5 = obj;
                                            lv_obj_set_pos(obj, 395, -23);
                                            lv_obj_set_size(obj, 179, 87);
                                            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xf4f6f9), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            {
                                                lv_obj_t *parent_obj = obj;
                                                {
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    objects.obj6 = obj;
                                                    lv_obj_set_pos(obj, -32, 41);
                                                    lv_obj_set_size(obj, 240, 22);
                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_ELASTIC);
                                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xf4f6f9), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    {
                                                        lv_obj_t *parent_obj = obj;
                                                        {
                                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                                            lv_obj_set_pos(obj, 40, -1);
                                                            lv_obj_set_size(obj, 72, 17);
                                                            lv_obj_set_style_text_font(obj, &ui_font_chinese18, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_label_set_text_static(obj, "系统电量:");
                                                        }
                                                        {
                                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                                            objects.obj64 = obj;
                                                            lv_obj_set_pos(obj, 101, -1);
                                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                            lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_label_set_text(obj, "");
                                                        }
                                                        {
                                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                                            lv_obj_set_pos(obj, 120, 0);
                                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                            lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_label_set_text_static(obj, "%");
                                                        }
                                                    }
                                                }
                                                {
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    objects.obj7 = obj;
                                                    lv_obj_set_pos(obj, -30, 27);
                                                    lv_obj_set_size(obj, 240, 13);
                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xf4f6f9), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    {
                                                        lv_obj_t *parent_obj = obj;
                                                        {
                                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                                            lv_obj_set_pos(obj, 39, -3);
                                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                            lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_label_set_text_static(obj, "TEM:");
                                                        }
                                                        {
                                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                                            objects.obj65 = obj;
                                                            lv_obj_set_pos(obj, 71, -3);
                                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                            lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_label_set_text(obj, "");
                                                        }
                                                        {
                                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                                            lv_obj_set_pos(obj, 102, -3);
                                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                            lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_label_set_text_static(obj, "HUM:");
                                                        }
                                                        {
                                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                                            objects.obj66 = obj;
                                                            lv_obj_set_pos(obj, 140, -3);
                                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                            lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_label_set_text(obj, "");
                                                        }
                                                    }
                                                }
                                                {
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    lv_obj_set_pos(obj, -32, 56);
                                                    lv_obj_set_size(obj, 240, 22);
                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_ELASTIC);
                                                    {
                                                        lv_obj_t *parent_obj = obj;
                                                        {
                                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                                            lv_obj_set_pos(obj, 40, -1);
                                                            lv_obj_set_size(obj, 72, 17);
                                                            lv_obj_set_style_text_font(obj, &ui_font_chinese18, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_label_set_text_static(obj, "连接状态:");
                                                        }
                                                        {
                                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                                            objects.obj67 = obj;
                                                            lv_obj_set_pos(obj, 101, -1);
                                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                            lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_label_set_text(obj, "");
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        {
                                            // ui_cont_cards
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            objects.ui_cont_cards = obj;
                                            lv_obj_set_pos(obj, 0, 55);
                                            lv_obj_set_size(obj, LV_PCT(100), 110);
                                            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            {
                                                lv_obj_t *parent_obj = obj;
                                                {
                                                    // ui_cont_card1
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    objects.ui_cont_card1 = obj;
                                                    lv_obj_set_pos(obj, 0, -15);
                                                    lv_obj_set_size(obj, LV_PCT(23), LV_PCT(100));
                                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_color(obj, lv_color_hex(0xe2e8f0), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_radius(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_COLUMN, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_row(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_left(obj, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    {
                                                        lv_obj_t *parent_obj = obj;
                                                        {
                                                            // ui_lbl_card1_title
                                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                                            objects.ui_lbl_card1_title = obj;
                                                            lv_obj_set_pos(obj, 13, 1);
                                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                            lv_obj_set_style_text_font(obj, &ui_font_chinese18, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_text_color(obj, lv_color_hex(0x718096), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_label_set_text_static(obj, "总物料种类\n(Categories)");
                                                        }
                                                        {
                                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                                            objects.obj8 = obj;
                                                            lv_obj_set_pos(obj, 0, 0);
                                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                            lv_obj_set_style_text_color(obj, lv_color_hex(0x1a202c), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_label_set_text(obj, "");
                                                        }
                                                    }
                                                }
                                                {
                                                    // ui_cont_card2
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    objects.ui_cont_card2 = obj;
                                                    lv_obj_set_pos(obj, -14, -20);
                                                    lv_obj_set_size(obj, LV_PCT(23), LV_PCT(100));
                                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_color(obj, lv_color_hex(0xe2e8f0), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_radius(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_COLUMN, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_row(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_left(obj, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    {
                                                        lv_obj_t *parent_obj = obj;
                                                        {
                                                            // ui_lbl_card1_title_1
                                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                                            objects.ui_lbl_card1_title_1 = obj;
                                                            lv_obj_set_pos(obj, 13, 1);
                                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                            lv_obj_set_style_text_font(obj, &ui_font_chinese18, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_text_color(obj, lv_color_hex(0x718096), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_label_set_text_static(obj, "总库存数\n(Total Stock)");
                                                        }
                                                        {
                                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                                            objects.obj9 = obj;
                                                            lv_obj_set_pos(obj, 0, 0);
                                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                            lv_obj_set_style_text_color(obj, lv_color_hex(0x1a202c), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_label_set_text(obj, "");
                                                        }
                                                    }
                                                }
                                                {
                                                    // ui_cont_card3
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    objects.ui_cont_card3 = obj;
                                                    lv_obj_set_pos(obj, 1, -28);
                                                    lv_obj_set_size(obj, LV_PCT(23), LV_PCT(100));
                                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_color(obj, lv_color_hex(0xe2e8f0), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_radius(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_COLUMN, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_row(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_left(obj, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    {
                                                        lv_obj_t *parent_obj = obj;
                                                        {
                                                            // ui_lbl_card1_title_2
                                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                                            objects.ui_lbl_card1_title_2 = obj;
                                                            lv_obj_set_pos(obj, 13, 1);
                                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                            lv_obj_set_style_text_font(obj, &ui_font_chinese18, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_text_color(obj, lv_color_hex(0x718096), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_label_set_text_static(obj, "库寸预警\n(Low Stock)");
                                                        }
                                                        {
                                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                                            objects.obj10 = obj;
                                                            lv_obj_set_pos(obj, 0, 0);
                                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                            lv_obj_set_style_text_color(obj, lv_color_hex(0x1a202c), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_label_set_text(obj, "");
                                                        }
                                                    }
                                                }
                                                {
                                                    // ui_cont_card4
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    objects.ui_cont_card4 = obj;
                                                    lv_obj_set_pos(obj, -9, -24);
                                                    lv_obj_set_size(obj, LV_PCT(23), LV_PCT(100));
                                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_color(obj, lv_color_hex(0xe2e8f0), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_radius(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_COLUMN, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_row(obj, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_left(obj, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    {
                                                        lv_obj_t *parent_obj = obj;
                                                        {
                                                            // ui_lbl_card1_title_3
                                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                                            objects.ui_lbl_card1_title_3 = obj;
                                                            lv_obj_set_pos(obj, 13, 1);
                                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                            lv_obj_set_style_text_font(obj, &ui_font_chinese18, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_text_color(obj, lv_color_hex(0x718096), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_label_set_text_static(obj, "仓库总容量\n(Capacity)");
                                                        }
                                                        {
                                                            // ui_bar_capacity
                                                            lv_obj_t *obj = lv_bar_create(parent_obj);
                                                            objects.ui_bar_capacity = obj;
                                                            lv_obj_set_pos(obj, 0, 0);
                                                            lv_obj_set_size(obj, LV_PCT(75), 12);
                                                            lv_obj_set_style_text_color(obj, lv_color_hex(0xe2e8f0), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xe9eaef), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0x3b82f6), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        {
                                            // ui_cont_header
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            objects.ui_cont_header = obj;
                                            lv_obj_set_pos(obj, -2, 9);
                                            lv_obj_set_size(obj, LV_PCT(100), 50);
                                            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_row(obj, 11, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_column(obj, 11, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_width(obj, 560, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_height(obj, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            {
                                                lv_obj_t *parent_obj = obj;
                                                {
                                                    // test3
                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                    objects.test3 = obj;
                                                    lv_obj_set_pos(obj, 10, 12);
                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                    lv_obj_set_style_text_font(obj, &ui_font_chinese24, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_label_set_text_static(obj, "实时数字化仓库看板");
                                                }
                                            }
                                        }
                                        {
                                            // ui_cont_bottom
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            objects.ui_cont_bottom = obj;
                                            lv_obj_set_pos(obj, -2, 175);
                                            lv_obj_set_size(obj, LV_PCT(100), 265);
                                            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_COLUMN, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            {
                                                lv_obj_t *parent_obj = obj;
                                                {
                                                    // ui_cont_board1
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    objects.ui_cont_board1 = obj;
                                                    lv_obj_set_pos(obj, 493, -147);
                                                    lv_obj_set_size(obj, LV_PCT(100), 125);
                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_radius(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_COLUMN, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_left(obj, 16, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_right(obj, 16, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_row(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    {
                                                        lv_obj_t *parent_obj = obj;
                                                        {
                                                            // ui_lbl_board1_title
                                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                                            objects.ui_lbl_board1_title = obj;
                                                            lv_obj_set_pos(obj, 0, 0);
                                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                            lv_obj_set_style_text_font(obj, &ui_font_chinese18, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_text_color(obj, lv_color_hex(0x1a202c), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_label_set_text_static(obj, "预警物料 (Low Stock Watchlist)");
                                                        }
                                                        {
                                                            // ui_cont_board1_list
                                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                                            objects.ui_cont_board1_list = obj;
                                                            lv_obj_set_pos(obj, 0, 0);
                                                            lv_obj_set_size(obj, LV_PCT(100), 18);
                                                            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            {
                                                                lv_obj_t *parent_obj = obj;
                                                                {
                                                                    // SKU
                                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                                    objects.sku = obj;
                                                                    lv_obj_set_pos(obj, 0, 0);
                                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                                    lv_obj_set_style_text_font(obj, &ui_font_chinese, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_text_color(obj, lv_color_hex(0x718096), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_label_set_text_static(obj, "编号 (SKU)");
                                                                }
                                                                {
                                                                    // NAME
                                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                                    objects.name = obj;
                                                                    lv_obj_set_pos(obj, 0, 0);
                                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                                    lv_obj_set_style_text_font(obj, &ui_font_chinese, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_text_color(obj, lv_color_hex(0x718096), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_label_set_text_static(obj, "特性(CHA)");
                                                                }
                                                                {
                                                                    // STOCK
                                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                                    objects.stock = obj;
                                                                    lv_obj_set_pos(obj, 0, 0);
                                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                                    lv_obj_set_style_text_font(obj, &ui_font_chinese, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_text_color(obj, lv_color_hex(0x718096), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_label_set_text_static(obj, "库存 (STOCK)");
                                                                }
                                                            }
                                                        }
                                                        {
                                                            // ui_cont_row1
                                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                                            objects.ui_cont_row1 = obj;
                                                            lv_obj_set_pos(obj, -14, -122);
                                                            lv_obj_set_size(obj, LV_PCT(100), 24);
                                                            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_AUTO);
                                                            lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_pad_row(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            {
                                                                lv_obj_t *parent_obj = obj;
                                                                {
                                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                                    lv_obj_set_pos(obj, 0, 0);
                                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                                    lv_label_set_text_static(obj, "ESP32-C3");
                                                                }
                                                                {
                                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                                    lv_obj_set_pos(obj, 0, 0);
                                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                                    lv_label_set_text_static(obj, "ESP32-C3");
                                                                }
                                                                {
                                                                    // ui_cont_badge1
                                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                                    objects.ui_cont_badge1 = obj;
                                                                    lv_obj_set_pos(obj, 0, 0);
                                                                    lv_obj_set_size(obj, 65, 18);
                                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xfee2e2), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_radius(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    {
                                                                        lv_obj_t *parent_obj = obj;
                                                                        {
                                                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                                                            objects.obj11 = obj;
                                                                            lv_obj_set_pos(obj, 0, 0);
                                                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                                            lv_obj_set_style_text_color(obj, lv_color_hex(0x9f0000), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                            lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                            lv_label_set_text_static(obj, "2 pcs");
                                                                        }
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                                {
                                                    // ui_cont_board2
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    objects.ui_cont_board2 = obj;
                                                    lv_obj_set_pos(obj, 0, 0);
                                                    lv_obj_set_size(obj, LV_PCT(100), 125);
                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_radius(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_COLUMN, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_left(obj, 16, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_right(obj, 16, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_row(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    {
                                                        lv_obj_t *parent_obj = obj;
                                                        {
                                                            // ui_lbl_board1_title_1
                                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                                            objects.ui_lbl_board1_title_1 = obj;
                                                            lv_obj_set_pos(obj, 0, 0);
                                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                            lv_obj_set_style_text_font(obj, &ui_font_chinese18, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_text_color(obj, lv_color_hex(0x1a202c), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_label_set_text_static(obj, "最新动态(Recent Activity)");
                                                        }
                                                        {
                                                            // ui_cont_board1_list_1
                                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                                            objects.ui_cont_board1_list_1 = obj;
                                                            lv_obj_set_pos(obj, 0, 0);
                                                            lv_obj_set_size(obj, LV_PCT(100), 18);
                                                            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            {
                                                                lv_obj_t *parent_obj = obj;
                                                                {
                                                                    // SKU_1
                                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                                    objects.sku_1 = obj;
                                                                    lv_obj_set_pos(obj, 0, 0);
                                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                                    lv_obj_set_style_text_font(obj, &ui_font_chinese, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_text_color(obj, lv_color_hex(0x718096), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_label_set_text_static(obj, "操作 (TYPE)");
                                                                }
                                                                {
                                                                    // NAME_1
                                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                                    objects.name_1 = obj;
                                                                    lv_obj_set_pos(obj, 0, 0);
                                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                                    lv_obj_set_style_text_font(obj, &ui_font_chinese, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_text_color(obj, lv_color_hex(0x718096), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_label_set_text_static(obj, "名称 (ITEM)");
                                                                }
                                                                {
                                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                                    objects.obj12 = obj;
                                                                    lv_obj_set_pos(obj, 0, 0);
                                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                                    lv_obj_set_style_text_font(obj, &ui_font_chinese, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_text_color(obj, lv_color_hex(0x718096), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_label_set_text_static(obj, "数量 (QTY)");
                                                                }
                                                                {
                                                                    // STOCK_1
                                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                                    objects.stock_1 = obj;
                                                                    lv_obj_set_pos(obj, 0, 0);
                                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                                    lv_obj_set_style_text_font(obj, &ui_font_chinese, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_text_color(obj, lv_color_hex(0x718096), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_label_set_text_static(obj, "时间 (TIME)");
                                                                }
                                                            }
                                                        }
                                                        {
                                                            // ui_cont_row2
                                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                                            objects.ui_cont_row2 = obj;
                                                            lv_obj_set_pos(obj, 0, 0);
                                                            lv_obj_set_size(obj, LV_PCT(100), 24);
                                                            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_AUTO);
                                                            lv_obj_set_style_pad_row(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            {
                                                                lv_obj_t *parent_obj = obj;
                                                                {
                                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                                    objects.obj13 = obj;
                                                                    lv_obj_set_pos(obj, -11, -54);
                                                                    lv_obj_set_size(obj, 50, 24);
                                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xfef3c7), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    {
                                                                        lv_obj_t *parent_obj = obj;
                                                                        {
                                                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                                                            objects.obj14 = obj;
                                                                            lv_obj_set_pos(obj, -11, -1);
                                                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                                            lv_obj_set_style_text_color(obj, lv_color_hex(0xd97706), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                            lv_obj_set_style_text_font(obj, &ui_font_chinese, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                            lv_label_set_text_static(obj, "OUT");
                                                                        }
                                                                    }
                                                                }
                                                                {
                                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                                    lv_obj_set_pos(obj, -22, 0);
                                                                    lv_obj_set_size(obj, LV_PCT(36), LV_SIZE_CONTENT);
                                                                    lv_label_set_text_static(obj, "              ESP32-C3");
                                                                }
                                                                {
                                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                                    lv_obj_set_pos(obj, -145, 1);
                                                                    lv_obj_set_size(obj, LV_PCT(20), LV_SIZE_CONTENT);
                                                                    lv_label_set_text_static(obj, "50");
                                                                }
                                                                {
                                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                                    lv_obj_set_pos(obj, 0, 0);
                                                                    lv_obj_set_size(obj, LV_PCT(15), LV_SIZE_CONTENT);
                                                                    lv_label_set_text_static(obj, "12.15");
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                {
                                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Tab2");
                                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_CHECKED);
                                    lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_COLUMN, LV_PART_MAIN | LV_STATE_CHECKED);
                                    lv_obj_set_style_pad_top(obj, 20, LV_PART_MAIN | LV_STATE_CHECKED);
                                    lv_obj_set_style_pad_bottom(obj, 20, LV_PART_MAIN | LV_STATE_CHECKED);
                                    lv_obj_set_style_pad_left(obj, 20, LV_PART_MAIN | LV_STATE_CHECKED);
                                    lv_obj_set_style_pad_right(obj, 20, LV_PART_MAIN | LV_STATE_CHECKED);
                                    lv_obj_set_style_pad_row(obj, 20, LV_PART_MAIN | LV_STATE_CHECKED);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // test3_1
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.test3_1 = obj;
                                            lv_obj_set_pos(obj, 0, 10);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_obj_set_style_text_font(obj, &ui_font_chinese24, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "库存可视化管理");
                                        }
                                        {
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            objects.obj15 = obj;
                                            lv_obj_set_pos(obj, 0, 50);
                                            lv_obj_set_size(obj, 530, 390);
                                            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_radius(obj, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW_WRAP, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_row(obj, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_top(obj, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_left(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            {
                                                lv_obj_t *parent_obj = obj;
                                                {
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    objects.obj16 = obj;
                                                    lv_obj_set_pos(obj, 964, 632);
                                                    lv_obj_set_size(obj, 312, 40);
                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                                    lv_obj_set_style_pad_top(obj, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_left(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_radius(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_color(obj, lv_color_hex(0xe2e8f0), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    {
                                                        lv_obj_t *parent_obj = obj;
                                                        {
                                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                                            objects.obj17 = obj;
                                                            lv_obj_set_pos(obj, 30, -8);
                                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                            lv_obj_set_style_text_font(obj, &ui_font_chinese, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_text_color(obj, lv_color_hex(0x979a9e), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_label_set_text_static(obj, "搜索SKU~分类或产品名称...");
                                                        }
                                                        {
                                                            lv_obj_t *obj = lv_image_create(parent_obj);
                                                            lv_obj_set_pos(obj, -7, -19);
                                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                            lv_image_set_src(obj, &img_page_search);
                                                            lv_image_set_scale(obj, 200);
                                                        }
                                                    }
                                                }
                                                {
                                                    lv_obj_t *obj = lv_list_create(parent_obj);
                                                    lv_obj_set_pos(obj, 3, 110);
                                                    lv_obj_set_size(obj, 512, 305);
                                                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW_WRAP, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_row(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_left(obj, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_column(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    {
                                                        lv_obj_t *parent_obj = obj;
                                                        {
                                                            // C1
                                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                                            objects.c1 = obj;
                                                            lv_obj_set_pos(obj, 374, -88);
                                                            lv_obj_set_size(obj, 231, 114);
                                                            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_add_event_cb(obj, event_handler_cb_page_main_c1, LV_EVENT_ALL, flowState);
                                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                                            lv_obj_set_style_pad_top(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_pad_left(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_pad_row(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_border_color(obj, lv_color_hex(0xe2e8f0), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_border_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_radius(obj, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_border_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            {
                                                                lv_obj_t *parent_obj = obj;
                                                                {
                                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                                    objects.obj18 = obj;
                                                                    lv_obj_set_pos(obj, 3, -4);
                                                                    lv_obj_set_size(obj, 102, 13);
                                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                                                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xe4ecf7), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_radius(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    {
                                                                        lv_obj_t *parent_obj = obj;
                                                                        {
                                                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                                                            objects.obj19 = obj;
                                                                            lv_obj_set_pos(obj, 0, 2);
                                                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_8, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                            lv_obj_set_style_text_color(obj, lv_color_hex(0x64748b), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                            lv_label_set_text(obj, "");
                                                                        }
                                                                    }
                                                                }
                                                                {
                                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                                    objects.obj20 = obj;
                                                                    lv_obj_set_pos(obj, 173, 2);
                                                                    lv_obj_set_size(obj, 36, 12);
                                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                                                                    lv_obj_set_style_radius(obj, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_text_color(obj, lv_color_hex(0x0a6c15), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_bg_opa(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    {
                                                                        lv_obj_t *parent_obj = obj;
                                                                        {
                                                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                                                            objects.obj21 = obj;
                                                                            lv_obj_set_pos(obj, 8, -1);
                                                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                                            lv_label_set_recolor(obj, true);
                                                                            lv_obj_set_style_text_font(obj, &ui_font_chinese10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                            lv_obj_set_style_text_color(obj, lv_color_hex(0x006c28), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                            lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                            lv_label_set_text(obj, "");
                                                                        }
                                                                    }
                                                                }
                                                                {
                                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                                    objects.obj22 = obj;
                                                                    lv_obj_set_pos(obj, 3, 20);
                                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                                    lv_obj_set_style_text_color(obj, lv_color_hex(0x212121), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_label_set_text(obj, "");
                                                                }
                                                                {
                                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                                    objects.obj23 = obj;
                                                                    lv_obj_set_pos(obj, 4, 45);
                                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_text_color(obj, lv_color_hex(0x64748b), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_label_set_text(obj, "");
                                                                }
                                                                {
                                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                                    objects.obj24 = obj;
                                                                    lv_obj_set_pos(obj, 3, 68);
                                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                                    lv_obj_set_style_text_font(obj, &ui_font_chinese8, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_text_color(obj, lv_color_hex(0x64748b), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_label_set_text_static(obj, "当前库存");
                                                                }
                                                                {
                                                                    lv_obj_t *obj = lv_bar_create(parent_obj);
                                                                    objects.obj68 = obj;
                                                                    lv_obj_set_pos(obj, 3, 81);
                                                                    lv_obj_set_size(obj, 204, 9);
                                                                    lv_bar_set_range(obj, 0, 100);
                                                                }
                                                                {
                                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                                    lv_obj_set_pos(obj, 137, 63);
                                                                    lv_obj_set_size(obj, 71, 23);
                                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    {
                                                                        lv_obj_t *parent_obj = obj;
                                                                        {
                                                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                                                            objects.obj69 = obj;
                                                                            lv_obj_set_pos(obj, 11, 2);
                                                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                                            lv_label_set_text(obj, "");
                                                                        }
                                                                        {
                                                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                                                            objects.obj25 = obj;
                                                                            lv_obj_set_pos(obj, 48, 6);
                                                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                                            lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                            lv_obj_set_style_text_color(obj, lv_color_hex(0x37598a), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                            lv_label_set_text_static(obj, "pcs");
                                                                        }
                                                                    }
                                                                }
                                                                {
                                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                                    lv_obj_set_pos(obj, 191, 21);
                                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                                    lv_label_set_text_static(obj, "1");
                                                                }
                                                                {
                                                                    // Active_Border_2 
                                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                                    objects.active_border_2_ = obj;
                                                                    lv_obj_set_pos(obj, -11, -11);
                                                                    lv_obj_set_size(obj, 231, 114);
                                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_border_width(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_border_color(obj, lv_color_hex(0x6366e7), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_border_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_radius(obj, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                }
                                                            }
                                                        }
                                                        {
                                                            // C1_1
                                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                                            objects.c1_1 = obj;
                                                            lv_obj_set_pos(obj, 374, -88);
                                                            lv_obj_set_size(obj, 231, 114);
                                                            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_add_event_cb(obj, event_handler_cb_page_main_c1_1, LV_EVENT_ALL, flowState);
                                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                                            lv_obj_set_style_pad_top(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_pad_left(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_pad_row(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_border_color(obj, lv_color_hex(0xe2e8f0), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_border_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_radius(obj, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_border_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            {
                                                                lv_obj_t *parent_obj = obj;
                                                                {
                                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                                    objects.obj26 = obj;
                                                                    lv_obj_set_pos(obj, 3, -4);
                                                                    lv_obj_set_size(obj, 102, 13);
                                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                                                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xe4ecf7), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_radius(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    {
                                                                        lv_obj_t *parent_obj = obj;
                                                                        {
                                                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                                                            objects.obj27 = obj;
                                                                            lv_obj_set_pos(obj, 0, 2);
                                                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_8, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                            lv_obj_set_style_text_color(obj, lv_color_hex(0x64748b), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                            lv_label_set_text(obj, "");
                                                                        }
                                                                    }
                                                                }
                                                                {
                                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                                    objects.obj28 = obj;
                                                                    lv_obj_set_pos(obj, 173, 2);
                                                                    lv_obj_set_size(obj, 36, 12);
                                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                                                                    lv_obj_set_style_radius(obj, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_text_color(obj, lv_color_hex(0x0a6c15), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_bg_opa(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    {
                                                                        lv_obj_t *parent_obj = obj;
                                                                        {
                                                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                                                            objects.obj29 = obj;
                                                                            lv_obj_set_pos(obj, 8, -1);
                                                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                                            lv_label_set_recolor(obj, true);
                                                                            lv_obj_set_style_text_font(obj, &ui_font_chinese10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                            lv_obj_set_style_text_color(obj, lv_color_hex(0x006c28), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                            lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                            lv_label_set_text(obj, "");
                                                                        }
                                                                    }
                                                                }
                                                                {
                                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                                    objects.obj30 = obj;
                                                                    lv_obj_set_pos(obj, 3, 20);
                                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                                    lv_obj_set_style_text_color(obj, lv_color_hex(0x212121), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_label_set_text(obj, "");
                                                                }
                                                                {
                                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                                    objects.obj31 = obj;
                                                                    lv_obj_set_pos(obj, 4, 45);
                                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_text_color(obj, lv_color_hex(0x64748b), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_label_set_text(obj, "");
                                                                }
                                                                {
                                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                                    objects.obj32 = obj;
                                                                    lv_obj_set_pos(obj, 3, 68);
                                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                                    lv_obj_set_style_text_font(obj, &ui_font_chinese8, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_text_color(obj, lv_color_hex(0x64748b), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_label_set_text_static(obj, "当前库存");
                                                                }
                                                                {
                                                                    lv_obj_t *obj = lv_bar_create(parent_obj);
                                                                    objects.obj70 = obj;
                                                                    lv_obj_set_pos(obj, 3, 81);
                                                                    lv_obj_set_size(obj, 204, 9);
                                                                    lv_bar_set_range(obj, 0, 100);
                                                                }
                                                                {
                                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                                    lv_obj_set_pos(obj, 137, 63);
                                                                    lv_obj_set_size(obj, 71, 23);
                                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    {
                                                                        lv_obj_t *parent_obj = obj;
                                                                        {
                                                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                                                            objects.obj71 = obj;
                                                                            lv_obj_set_pos(obj, 11, 2);
                                                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                                            lv_label_set_text(obj, "");
                                                                        }
                                                                        {
                                                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                                                            objects.obj33 = obj;
                                                                            lv_obj_set_pos(obj, 48, 6);
                                                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                                            lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                            lv_obj_set_style_text_color(obj, lv_color_hex(0x37598a), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                            lv_label_set_text_static(obj, "pcs");
                                                                        }
                                                                    }
                                                                }
                                                                {
                                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                                    lv_obj_set_pos(obj, 191, 21);
                                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                                    lv_label_set_text_static(obj, "2");
                                                                }
                                                                {
                                                                    // Active_Border_3
                                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                                    objects.active_border_3 = obj;
                                                                    lv_obj_set_pos(obj, -11, -11);
                                                                    lv_obj_set_size(obj, 231, 114);
                                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_border_width(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_border_color(obj, lv_color_hex(0x6366e7), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_border_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_radius(obj, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                }
                                                            }
                                                        }
                                                        {
                                                            // C1_2
                                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                                            objects.c1_2 = obj;
                                                            lv_obj_set_pos(obj, 374, -88);
                                                            lv_obj_set_size(obj, 231, 114);
                                                            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_add_event_cb(obj, event_handler_cb_page_main_c1_2, LV_EVENT_ALL, flowState);
                                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                                            lv_obj_set_style_pad_top(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_pad_left(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_pad_row(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_border_color(obj, lv_color_hex(0xe2e8f0), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_border_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_radius(obj, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_border_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            {
                                                                lv_obj_t *parent_obj = obj;
                                                                {
                                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                                    objects.obj34 = obj;
                                                                    lv_obj_set_pos(obj, 3, -4);
                                                                    lv_obj_set_size(obj, 102, 13);
                                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                                                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xe4ecf7), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_radius(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    {
                                                                        lv_obj_t *parent_obj = obj;
                                                                        {
                                                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                                                            objects.obj35 = obj;
                                                                            lv_obj_set_pos(obj, 0, 2);
                                                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_8, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                            lv_obj_set_style_text_color(obj, lv_color_hex(0x64748b), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                            lv_label_set_text(obj, "");
                                                                        }
                                                                    }
                                                                }
                                                                {
                                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                                    objects.obj36 = obj;
                                                                    lv_obj_set_pos(obj, 173, 2);
                                                                    lv_obj_set_size(obj, 36, 12);
                                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                                                                    lv_obj_set_style_radius(obj, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_text_color(obj, lv_color_hex(0x0a6c15), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_bg_opa(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    {
                                                                        lv_obj_t *parent_obj = obj;
                                                                        {
                                                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                                                            objects.obj37 = obj;
                                                                            lv_obj_set_pos(obj, 8, -1);
                                                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                                            lv_label_set_recolor(obj, true);
                                                                            lv_obj_set_style_text_font(obj, &ui_font_chinese10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                            lv_obj_set_style_text_color(obj, lv_color_hex(0x006c28), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                            lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                            lv_label_set_text(obj, "");
                                                                        }
                                                                    }
                                                                }
                                                                {
                                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                                    objects.obj38 = obj;
                                                                    lv_obj_set_pos(obj, 3, 20);
                                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                                    lv_obj_set_style_text_color(obj, lv_color_hex(0x212121), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_label_set_text(obj, "");
                                                                }
                                                                {
                                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                                    objects.obj39 = obj;
                                                                    lv_obj_set_pos(obj, 4, 45);
                                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_text_color(obj, lv_color_hex(0x64748b), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_label_set_text(obj, "");
                                                                }
                                                                {
                                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                                    objects.obj40 = obj;
                                                                    lv_obj_set_pos(obj, 3, 68);
                                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                                    lv_obj_set_style_text_font(obj, &ui_font_chinese8, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_text_color(obj, lv_color_hex(0x64748b), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_label_set_text_static(obj, "当前库存");
                                                                }
                                                                {
                                                                    lv_obj_t *obj = lv_bar_create(parent_obj);
                                                                    objects.obj72 = obj;
                                                                    lv_obj_set_pos(obj, 3, 81);
                                                                    lv_obj_set_size(obj, 204, 9);
                                                                    lv_bar_set_range(obj, 0, 100);
                                                                }
                                                                {
                                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                                    lv_obj_set_pos(obj, 137, 63);
                                                                    lv_obj_set_size(obj, 71, 23);
                                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    {
                                                                        lv_obj_t *parent_obj = obj;
                                                                        {
                                                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                                                            objects.obj73 = obj;
                                                                            lv_obj_set_pos(obj, 11, 2);
                                                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                                            lv_label_set_text(obj, "");
                                                                        }
                                                                        {
                                                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                                                            objects.obj41 = obj;
                                                                            lv_obj_set_pos(obj, 48, 6);
                                                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                                            lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                            lv_obj_set_style_text_color(obj, lv_color_hex(0x37598a), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                            lv_label_set_text_static(obj, "pcs");
                                                                        }
                                                                    }
                                                                }
                                                                {
                                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                                    lv_obj_set_pos(obj, 191, 21);
                                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                                    lv_label_set_text_static(obj, "3");
                                                                }
                                                                {
                                                                    // Active_Border_4
                                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                                    objects.active_border_4 = obj;
                                                                    lv_obj_set_pos(obj, -11, -11);
                                                                    lv_obj_set_size(obj, 231, 114);
                                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_border_width(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_border_color(obj, lv_color_hex(0x6366e7), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_border_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_radius(obj, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                }
                                                            }
                                                        }
                                                        {
                                                            // C1_3
                                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                                            objects.c1_3 = obj;
                                                            lv_obj_set_pos(obj, 374, -88);
                                                            lv_obj_set_size(obj, 231, 114);
                                                            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_add_event_cb(obj, event_handler_cb_page_main_c1_3, LV_EVENT_ALL, flowState);
                                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                                            lv_obj_set_style_pad_top(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_pad_left(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_pad_row(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_border_color(obj, lv_color_hex(0xe2e8f0), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_border_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_radius(obj, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_border_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            {
                                                                lv_obj_t *parent_obj = obj;
                                                                {
                                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                                    objects.obj42 = obj;
                                                                    lv_obj_set_pos(obj, 3, -4);
                                                                    lv_obj_set_size(obj, 102, 13);
                                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                                                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xe4ecf7), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_radius(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    {
                                                                        lv_obj_t *parent_obj = obj;
                                                                        {
                                                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                                                            objects.obj43 = obj;
                                                                            lv_obj_set_pos(obj, 0, 2);
                                                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_8, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                            lv_obj_set_style_text_color(obj, lv_color_hex(0x64748b), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                            lv_label_set_text(obj, "");
                                                                        }
                                                                    }
                                                                }
                                                                {
                                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                                    objects.obj44 = obj;
                                                                    lv_obj_set_pos(obj, 173, 2);
                                                                    lv_obj_set_size(obj, 36, 12);
                                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                                                                    lv_obj_set_style_radius(obj, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_text_color(obj, lv_color_hex(0x0a6c15), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_bg_opa(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    {
                                                                        lv_obj_t *parent_obj = obj;
                                                                        {
                                                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                                                            objects.obj45 = obj;
                                                                            lv_obj_set_pos(obj, 8, -1);
                                                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                                            lv_label_set_recolor(obj, true);
                                                                            lv_obj_set_style_text_font(obj, &ui_font_chinese10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                            lv_obj_set_style_text_color(obj, lv_color_hex(0x006c28), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                            lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                            lv_label_set_text(obj, "");
                                                                        }
                                                                    }
                                                                }
                                                                {
                                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                                    objects.obj46 = obj;
                                                                    lv_obj_set_pos(obj, 3, 20);
                                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                                    lv_obj_set_style_text_color(obj, lv_color_hex(0x212121), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_label_set_text(obj, "");
                                                                }
                                                                {
                                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                                    objects.obj47 = obj;
                                                                    lv_obj_set_pos(obj, 4, 45);
                                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_text_color(obj, lv_color_hex(0x64748b), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_label_set_text(obj, "");
                                                                }
                                                                {
                                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                                    objects.obj48 = obj;
                                                                    lv_obj_set_pos(obj, 3, 68);
                                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                                    lv_obj_set_style_text_font(obj, &ui_font_chinese8, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_text_color(obj, lv_color_hex(0x64748b), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_label_set_text_static(obj, "当前库存");
                                                                }
                                                                {
                                                                    lv_obj_t *obj = lv_bar_create(parent_obj);
                                                                    objects.obj74 = obj;
                                                                    lv_obj_set_pos(obj, 3, 81);
                                                                    lv_obj_set_size(obj, 204, 9);
                                                                    lv_bar_set_range(obj, 0, 100);
                                                                }
                                                                {
                                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                                    lv_obj_set_pos(obj, 137, 63);
                                                                    lv_obj_set_size(obj, 71, 23);
                                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    {
                                                                        lv_obj_t *parent_obj = obj;
                                                                        {
                                                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                                                            objects.obj75 = obj;
                                                                            lv_obj_set_pos(obj, 11, 2);
                                                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                                            lv_label_set_text(obj, "");
                                                                        }
                                                                        {
                                                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                                                            objects.obj49 = obj;
                                                                            lv_obj_set_pos(obj, 48, 6);
                                                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                                            lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                            lv_obj_set_style_text_color(obj, lv_color_hex(0x37598a), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                            lv_label_set_text_static(obj, "pcs");
                                                                        }
                                                                    }
                                                                }
                                                                {
                                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                                    lv_obj_set_pos(obj, 191, 21);
                                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                                    lv_label_set_text_static(obj, "4");
                                                                }
                                                                {
                                                                    // Active_Border_5
                                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                                    objects.active_border_5 = obj;
                                                                    lv_obj_set_pos(obj, -11, -11);
                                                                    lv_obj_set_size(obj, 231, 114);
                                                                    lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_border_width(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_border_color(obj, lv_color_hex(0x6366e7), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_border_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_radius(obj, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                }
                                                            }
                                                        }
                                                        {
                                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                                            objects.obj76 = obj;
                                                            lv_obj_set_pos(obj, 227, 376);
                                                            lv_obj_set_size(obj, 292, 61);
                                                            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            {
                                                                lv_obj_t *parent_obj = obj;
                                                                {
                                                                    lv_obj_t *obj = lv_slider_create(parent_obj);
                                                                    objects.obj50 = obj;
                                                                    lv_obj_set_pos(obj, 0, 30);
                                                                    lv_obj_set_size(obj, 150, 10);
                                                                    lv_slider_set_range(obj, -100, 100);
                                                                    lv_obj_add_event_cb(obj, event_handler_cb_page_main_obj50, LV_EVENT_ALL, flowState);
                                                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0x0ea5e9), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
                                                                }
                                                                {
                                                                    lv_obj_t *obj = lv_button_create(parent_obj);
                                                                    objects.obj51 = obj;
                                                                    lv_obj_set_pos(obj, 164, -3);
                                                                    lv_obj_set_size(obj, 100, 50);
                                                                    lv_obj_add_event_cb(obj, event_handler_cb_page_main_obj51, LV_EVENT_ALL, flowState);
                                                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0x1e3a8a), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    {
                                                                        lv_obj_t *parent_obj = obj;
                                                                        {
                                                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                                                            objects.obj52 = obj;
                                                                            lv_obj_set_pos(obj, 0, 0);
                                                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                            lv_obj_set_style_text_font(obj, &ui_font_chinese18, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                            lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                            lv_label_set_text_static(obj, "一键出入");
                                                                        }
                                                                    }
                                                                }
                                                                {
                                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                                    objects.obj53 = obj;
                                                                    lv_obj_set_pos(obj, 1, 4);
                                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                                    lv_obj_set_style_text_font(obj, &ui_font_chinese18, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_text_color(obj, lv_color_hex(0x1e293b), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_label_set_text(obj, "");
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                {
                                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "Tab3");
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            objects.obj54 = obj;
                                            lv_obj_set_pos(obj, 0, 50);
                                            lv_obj_set_size(obj, 530, 390);
                                            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_radius(obj, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_layout(obj, LV_LAYOUT_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_row(obj, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_top(obj, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_left(obj, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_layout(obj, LV_LAYOUT_NONE, LV_PART_INDICATOR | LV_STATE_DEFAULT);
                                            lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW, LV_PART_INDICATOR | LV_STATE_DEFAULT);
                                            {
                                                lv_obj_t *parent_obj = obj;
                                                {
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    objects.obj55 = obj;
                                                    lv_obj_set_pos(obj, -8, -2);
                                                    lv_obj_set_size(obj, 312, 40);
                                                    lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                                    lv_obj_set_style_pad_top(obj, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_left(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_radius(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_color(obj, lv_color_hex(0xe2e8f0), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    {
                                                        lv_obj_t *parent_obj = obj;
                                                        {
                                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                                            objects.obj56 = obj;
                                                            lv_obj_set_pos(obj, 30, -8);
                                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                            lv_obj_set_style_text_font(obj, &ui_font_chinese, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_text_color(obj, lv_color_hex(0x979a9e), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_label_set_text_static(obj, "搜索SKU~分类或产品名称...");
                                                        }
                                                        {
                                                            lv_obj_t *obj = lv_image_create(parent_obj);
                                                            lv_obj_set_pos(obj, -7, -19);
                                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                            lv_image_set_src(obj, &img_page_search);
                                                            lv_image_set_scale(obj, 200);
                                                        }
                                                    }
                                                }
                                                {
                                                    // List-material
                                                    lv_obj_t *obj = lv_list_create(parent_obj);
                                                    objects.list_material = obj;
                                                    lv_obj_set_pos(obj, -16, 47);
                                                    lv_obj_set_size(obj, 518, 309);
                                                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_COLUMN, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_top(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_left(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_pad_column(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    {
                                                        lv_obj_t *parent_obj = obj;
                                                        {
                                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                                            lv_obj_set_pos(obj, -13, 10);
                                                            lv_obj_set_size(obj, 499, 20);
                                                            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            {
                                                                lv_obj_t *parent_obj = obj;
                                                                {
                                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                                    objects.obj57 = obj;
                                                                    lv_obj_set_pos(obj, 5, 2);
                                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                                    lv_obj_set_style_text_color(obj, lv_color_hex(0x64748b), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_text_font(obj, &ui_font_chinese, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_label_set_text_static(obj, "时间(TIME)");
                                                                }
                                                                {
                                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                                    objects.obj58 = obj;
                                                                    lv_obj_set_pos(obj, 146, 0);
                                                                    lv_obj_set_size(obj, LV_PCT(20), LV_SIZE_CONTENT);
                                                                    lv_obj_set_style_text_color(obj, lv_color_hex(0x64748b), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_text_font(obj, &ui_font_chinese, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_label_set_text_static(obj, "操作(ACTION)");
                                                                }
                                                                {
                                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                                    objects.obj59 = obj;
                                                                    lv_obj_set_pos(obj, 255, 0);
                                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                                    lv_obj_set_style_text_color(obj, lv_color_hex(0x64748b), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_text_font(obj, &ui_font_chinese, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_label_set_text_static(obj, "名称(NAME)");
                                                                }
                                                                {
                                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                                    objects.obj60 = obj;
                                                                    lv_obj_set_pos(obj, 354, 0);
                                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                                    lv_obj_set_style_text_color(obj, lv_color_hex(0x64748b), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_text_font(obj, &ui_font_chinese, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_label_set_text_static(obj, "数量(QTY)");
                                                                }
                                                                {
                                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                                    objects.obj61 = obj;
                                                                    lv_obj_set_pos(obj, 452, 0);
                                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                                    lv_obj_set_style_text_color(obj, lv_color_hex(0x64748b), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_obj_set_style_text_font(obj, &ui_font_chinese, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                                    lv_label_set_text_static(obj, "USER");
                                                                }
                                                            }
                                                        }
                                                        {
                                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                                            lv_obj_set_pos(obj, -8, 39);
                                                            lv_obj_set_size(obj, 490, 19);
                                                            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                            {
                                                                lv_obj_t *parent_obj = obj;
                                                                {
                                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                                    objects.obj77 = obj;
                                                                    lv_obj_set_pos(obj, 409, 128);
                                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                                    lv_label_set_text(obj, "");
                                                                }
                                                                {
                                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                                    objects.obj78 = obj;
                                                                    lv_obj_set_pos(obj, 412, 183);
                                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                                    lv_label_set_text(obj, "");
                                                                }
                                                                {
                                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                                    objects.obj79 = obj;
                                                                    lv_obj_set_pos(obj, 414, 123);
                                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                                    lv_label_set_text(obj, "");
                                                                }
                                                                {
                                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                                    objects.obj80 = obj;
                                                                    lv_obj_set_pos(obj, 395, 126);
                                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                                    lv_label_set_text(obj, "");
                                                                }
                                                                {
                                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                                    objects.obj81 = obj;
                                                                    lv_obj_set_pos(obj, 381, 135);
                                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                                    lv_label_set_text(obj, "");
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        {
                                            lv_obj_t *obj = lv_button_create(parent_obj);
                                            objects.obj62 = obj;
                                            lv_obj_set_pos(obj, 378, 0);
                                            lv_obj_set_size(obj, 132, 39);
                                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            {
                                                lv_obj_t *parent_obj = obj;
                                                {
                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                    objects.obj63 = obj;
                                                    lv_obj_set_pos(obj, 14, 1);
                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_text_font(obj, &ui_font_chinese18, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_text_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_text_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_label_set_text_static(obj, "打印记录清单");
                                                }
                                                {
                                                    lv_obj_t *obj = lv_image_create(parent_obj);
                                                    lv_obj_set_pos(obj, -13, -13);
                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                    lv_image_set_src(obj, &img_page_printfer);
                                                    lv_image_set_scale(obj, 150);
                                                }
                                            }
                                        }
                                        {
                                            // test3_2
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.test3_2 = obj;
                                            lv_obj_set_pos(obj, 4, 9);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_obj_set_style_text_font(obj, &ui_font_chinese24, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "实时出入记录");
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        {
            lv_obj_t *obj = lv_image_create(parent_obj);
            lv_obj_set_pos(obj, 15, 413);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_page_uimode);
        }
    }
    
    tick_screen_page_main();
}

void tick_screen_page_main() {
    void *flowState = getFlowState(0, 1);
    (void)flowState;
    {
        bool new_val = evalBooleanProperty(flowState, 4, 3, "Failed to evaluate Checked state");
        bool cur_val = lv_obj_has_state(objects.dashboard, LV_STATE_CHECKED);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.dashboard;
            if (new_val) {
                lv_obj_add_state(objects.dashboard, LV_STATE_CHECKED);
            } else {
                lv_obj_remove_state(objects.dashboard, LV_STATE_CHECKED);
            }
            tick_value_change_obj = NULL;
        }
    }
    {
        bool new_val = evalBooleanProperty(flowState, 7, 3, "Failed to evaluate Checked state");
        bool cur_val = lv_obj_has_state(objects.dashboard_1, LV_STATE_CHECKED);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.dashboard_1;
            if (new_val) {
                lv_obj_add_state(objects.dashboard_1, LV_STATE_CHECKED);
            } else {
                lv_obj_remove_state(objects.dashboard_1, LV_STATE_CHECKED);
            }
            tick_value_change_obj = NULL;
        }
    }
    {
        bool new_val = evalBooleanProperty(flowState, 10, 3, "Failed to evaluate Checked state");
        bool cur_val = lv_obj_has_state(objects.dashboard_2, LV_STATE_CHECKED);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.dashboard_2;
            if (new_val) {
                lv_obj_add_state(objects.dashboard_2, LV_STATE_CHECKED);
            } else {
                lv_obj_remove_state(objects.dashboard_2, LV_STATE_CHECKED);
            }
            tick_value_change_obj = NULL;
        }
    }
    {
        int32_t new_val = evalIntegerProperty(flowState, 14, 3, "Failed to evaluate Active tab in Tabview widget");
        int32_t cur_val = lv_tabview_get_tab_active(objects.obj3);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.obj3;
            lv_tabview_set_active(objects.obj3, new_val, LV_ANIM_OFF);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = evalTextProperty(flowState, 20, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.obj64);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.obj64;
            lv_label_set_text(objects.obj64, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = evalTextProperty(flowState, 24, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.obj65);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.obj65;
            lv_label_set_text(objects.obj65, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = evalTextProperty(flowState, 26, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.obj66);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.obj66;
            lv_label_set_text(objects.obj66, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = evalTextProperty(flowState, 29, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.obj67);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.obj67;
            lv_label_set_text(objects.obj67, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = evalTextProperty(flowState, 33, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.obj8);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.obj8;
            lv_label_set_text(objects.obj8, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = evalTextProperty(flowState, 36, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.obj9);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.obj9;
            lv_label_set_text(objects.obj9, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = evalTextProperty(flowState, 39, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.obj10);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.obj10;
            lv_label_set_text(objects.obj10, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        int32_t new_val = evalIntegerProperty(flowState, 43, 3, "Failed to evaluate Value in Bar widget");
        int32_t cur_val = lv_bar_get_value(objects.ui_bar_capacity);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.ui_bar_capacity;
            lv_bar_set_value(objects.ui_bar_capacity, new_val, LV_ANIM_OFF);
            tick_value_change_obj = NULL;
        }
    }
    {
        bool new_val = evalBooleanProperty(flowState, 77, 3, "Failed to evaluate Hidden flag");
        bool cur_val = lv_obj_has_flag(objects.c1, LV_OBJ_FLAG_HIDDEN);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.c1;
            if (new_val) {
                lv_obj_add_flag(objects.c1, LV_OBJ_FLAG_HIDDEN);
            } else {
                lv_obj_remove_flag(objects.c1, LV_OBJ_FLAG_HIDDEN);
            }
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = evalTextProperty(flowState, 79, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.obj19);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.obj19;
            lv_label_set_text(objects.obj19, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = evalTextProperty(flowState, 81, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.obj21);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.obj21;
            lv_label_set_text(objects.obj21, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = evalTextProperty(flowState, 82, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.obj22);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.obj22;
            lv_label_set_text(objects.obj22, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = evalTextProperty(flowState, 83, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.obj23);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.obj23;
            lv_label_set_text(objects.obj23, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        int32_t new_val = evalIntegerProperty(flowState, 84, 3, "Failed to evaluate Max in Bar widget");
        int32_t cur_val = lv_bar_get_max_value(objects.obj68);
        if (new_val != cur_val) {
            int16_t min = lv_bar_get_min_value(objects.obj68);
            int16_t max = new_val;
            if (min < max) {
                lv_bar_set_range(objects.obj68, min, max);
            }
        }
    }
    {
        int32_t new_val = evalIntegerProperty(flowState, 84, 4, "Failed to evaluate Value in Bar widget");
        int32_t cur_val = lv_bar_get_value(objects.obj68);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.obj68;
            lv_bar_set_value(objects.obj68, new_val, LV_ANIM_OFF);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = evalTextProperty(flowState, 87, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.obj69);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.obj69;
            lv_label_set_text(objects.obj69, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        bool new_val = evalBooleanProperty(flowState, 90, 3, "Failed to evaluate Hidden flag");
        bool cur_val = lv_obj_has_flag(objects.active_border_2_, LV_OBJ_FLAG_HIDDEN);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.active_border_2_;
            if (new_val) {
                lv_obj_add_flag(objects.active_border_2_, LV_OBJ_FLAG_HIDDEN);
            } else {
                lv_obj_remove_flag(objects.active_border_2_, LV_OBJ_FLAG_HIDDEN);
            }
            tick_value_change_obj = NULL;
        }
    }
    {
        bool new_val = evalBooleanProperty(flowState, 91, 3, "Failed to evaluate Hidden flag");
        bool cur_val = lv_obj_has_flag(objects.c1_1, LV_OBJ_FLAG_HIDDEN);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.c1_1;
            if (new_val) {
                lv_obj_add_flag(objects.c1_1, LV_OBJ_FLAG_HIDDEN);
            } else {
                lv_obj_remove_flag(objects.c1_1, LV_OBJ_FLAG_HIDDEN);
            }
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = evalTextProperty(flowState, 93, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.obj27);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.obj27;
            lv_label_set_text(objects.obj27, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = evalTextProperty(flowState, 95, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.obj29);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.obj29;
            lv_label_set_text(objects.obj29, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = evalTextProperty(flowState, 96, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.obj30);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.obj30;
            lv_label_set_text(objects.obj30, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = evalTextProperty(flowState, 97, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.obj31);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.obj31;
            lv_label_set_text(objects.obj31, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        int32_t new_val = evalIntegerProperty(flowState, 99, 3, "Failed to evaluate Max in Bar widget");
        int32_t cur_val = lv_bar_get_max_value(objects.obj70);
        if (new_val != cur_val) {
            int16_t min = lv_bar_get_min_value(objects.obj70);
            int16_t max = new_val;
            if (min < max) {
                lv_bar_set_range(objects.obj70, min, max);
            }
        }
    }
    {
        int32_t new_val = evalIntegerProperty(flowState, 99, 4, "Failed to evaluate Value in Bar widget");
        int32_t cur_val = lv_bar_get_value(objects.obj70);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.obj70;
            lv_bar_set_value(objects.obj70, new_val, LV_ANIM_OFF);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = evalTextProperty(flowState, 101, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.obj71);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.obj71;
            lv_label_set_text(objects.obj71, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        bool new_val = evalBooleanProperty(flowState, 104, 3, "Failed to evaluate Hidden flag");
        bool cur_val = lv_obj_has_flag(objects.active_border_3, LV_OBJ_FLAG_HIDDEN);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.active_border_3;
            if (new_val) {
                lv_obj_add_flag(objects.active_border_3, LV_OBJ_FLAG_HIDDEN);
            } else {
                lv_obj_remove_flag(objects.active_border_3, LV_OBJ_FLAG_HIDDEN);
            }
            tick_value_change_obj = NULL;
        }
    }
    {
        bool new_val = evalBooleanProperty(flowState, 105, 3, "Failed to evaluate Hidden flag");
        bool cur_val = lv_obj_has_flag(objects.c1_2, LV_OBJ_FLAG_HIDDEN);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.c1_2;
            if (new_val) {
                lv_obj_add_flag(objects.c1_2, LV_OBJ_FLAG_HIDDEN);
            } else {
                lv_obj_remove_flag(objects.c1_2, LV_OBJ_FLAG_HIDDEN);
            }
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = evalTextProperty(flowState, 107, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.obj35);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.obj35;
            lv_label_set_text(objects.obj35, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = evalTextProperty(flowState, 109, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.obj37);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.obj37;
            lv_label_set_text(objects.obj37, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = evalTextProperty(flowState, 110, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.obj38);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.obj38;
            lv_label_set_text(objects.obj38, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = evalTextProperty(flowState, 111, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.obj39);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.obj39;
            lv_label_set_text(objects.obj39, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        int32_t new_val = evalIntegerProperty(flowState, 113, 3, "Failed to evaluate Max in Bar widget");
        int32_t cur_val = lv_bar_get_max_value(objects.obj72);
        if (new_val != cur_val) {
            int16_t min = lv_bar_get_min_value(objects.obj72);
            int16_t max = new_val;
            if (min < max) {
                lv_bar_set_range(objects.obj72, min, max);
            }
        }
    }
    {
        int32_t new_val = evalIntegerProperty(flowState, 113, 4, "Failed to evaluate Value in Bar widget");
        int32_t cur_val = lv_bar_get_value(objects.obj72);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.obj72;
            lv_bar_set_value(objects.obj72, new_val, LV_ANIM_OFF);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = evalTextProperty(flowState, 115, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.obj73);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.obj73;
            lv_label_set_text(objects.obj73, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        bool new_val = evalBooleanProperty(flowState, 118, 3, "Failed to evaluate Hidden flag");
        bool cur_val = lv_obj_has_flag(objects.active_border_4, LV_OBJ_FLAG_HIDDEN);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.active_border_4;
            if (new_val) {
                lv_obj_add_flag(objects.active_border_4, LV_OBJ_FLAG_HIDDEN);
            } else {
                lv_obj_remove_flag(objects.active_border_4, LV_OBJ_FLAG_HIDDEN);
            }
            tick_value_change_obj = NULL;
        }
    }
    {
        bool new_val = evalBooleanProperty(flowState, 119, 3, "Failed to evaluate Hidden flag");
        bool cur_val = lv_obj_has_flag(objects.c1_3, LV_OBJ_FLAG_HIDDEN);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.c1_3;
            if (new_val) {
                lv_obj_add_flag(objects.c1_3, LV_OBJ_FLAG_HIDDEN);
            } else {
                lv_obj_remove_flag(objects.c1_3, LV_OBJ_FLAG_HIDDEN);
            }
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = evalTextProperty(flowState, 121, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.obj43);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.obj43;
            lv_label_set_text(objects.obj43, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = evalTextProperty(flowState, 123, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.obj45);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.obj45;
            lv_label_set_text(objects.obj45, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = evalTextProperty(flowState, 124, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.obj46);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.obj46;
            lv_label_set_text(objects.obj46, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = evalTextProperty(flowState, 125, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.obj47);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.obj47;
            lv_label_set_text(objects.obj47, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        int32_t new_val = evalIntegerProperty(flowState, 126, 3, "Failed to evaluate Max in Bar widget");
        int32_t cur_val = lv_bar_get_max_value(objects.obj74);
        if (new_val != cur_val) {
            int16_t min = lv_bar_get_min_value(objects.obj74);
            int16_t max = new_val;
            if (min < max) {
                lv_bar_set_range(objects.obj74, min, max);
            }
        }
    }
    {
        int32_t new_val = evalIntegerProperty(flowState, 126, 4, "Failed to evaluate Value in Bar widget");
        int32_t cur_val = lv_bar_get_value(objects.obj74);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.obj74;
            lv_bar_set_value(objects.obj74, new_val, LV_ANIM_OFF);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = evalTextProperty(flowState, 129, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.obj75);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.obj75;
            lv_label_set_text(objects.obj75, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        bool new_val = evalBooleanProperty(flowState, 132, 3, "Failed to evaluate Hidden flag");
        bool cur_val = lv_obj_has_flag(objects.active_border_5, LV_OBJ_FLAG_HIDDEN);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.active_border_5;
            if (new_val) {
                lv_obj_add_flag(objects.active_border_5, LV_OBJ_FLAG_HIDDEN);
            } else {
                lv_obj_remove_flag(objects.active_border_5, LV_OBJ_FLAG_HIDDEN);
            }
            tick_value_change_obj = NULL;
        }
    }
    {
        bool new_val = evalBooleanProperty(flowState, 133, 3, "Failed to evaluate Hidden flag");
        bool cur_val = lv_obj_has_flag(objects.obj76, LV_OBJ_FLAG_HIDDEN);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.obj76;
            if (new_val) {
                lv_obj_add_flag(objects.obj76, LV_OBJ_FLAG_HIDDEN);
            } else {
                lv_obj_remove_flag(objects.obj76, LV_OBJ_FLAG_HIDDEN);
            }
            tick_value_change_obj = NULL;
        }
    }
    {
        int32_t new_val = evalIntegerProperty(flowState, 134, 3, "Failed to evaluate Value in Slider widget");
        int32_t cur_val = lv_slider_get_value(objects.obj50);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.obj50;
            lv_slider_set_value(objects.obj50, new_val, LV_ANIM_OFF);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = evalTextProperty(flowState, 137, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.obj53);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.obj53;
            lv_label_set_text(objects.obj53, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = evalTextProperty(flowState, 151, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.obj77);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.obj77;
            lv_label_set_text(objects.obj77, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = evalTextProperty(flowState, 152, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.obj78);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.obj78;
            lv_label_set_text(objects.obj78, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = evalTextProperty(flowState, 153, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.obj79);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.obj79;
            lv_label_set_text(objects.obj79, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = evalTextProperty(flowState, 154, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.obj80);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.obj80;
            lv_label_set_text(objects.obj80, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = evalTextProperty(flowState, 155, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.obj81);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.obj81;
            lv_label_set_text(objects.obj81, new_val);
            tick_value_change_obj = NULL;
        }
    }
}

void create_screen_page_error() {
    void *flowState = getFlowState(0, 2);
    (void)flowState;
    lv_obj_t *obj = lv_obj_create(0);
    objects.page_error = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 800, 480);
    
    tick_screen_page_error();
}

void tick_screen_page_error() {
    void *flowState = getFlowState(0, 2);
    (void)flowState;
}

typedef void (*tick_screen_func_t)();
tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_page_lock,
    tick_screen_page_main,
    tick_screen_page_error,
};
void tick_screen(int screen_index) {
    if (screen_index >= 0 && screen_index < 3) {
        tick_screen_funcs[screen_index]();
    }
}
void tick_screen_by_id(enum ScreensEnum screenId) {
    tick_screen(screenId - 1);
}

//
// Fonts
//

ext_font_desc_t fonts[] = {
    { "Chinese", &ui_font_chinese },
    { "Chinese24", &ui_font_chinese24 },
    { "Chinese18", &ui_font_chinese18 },
    { "Chinese10", &ui_font_chinese10 },
    { "Chinese8", &ui_font_chinese8 },
#if LV_FONT_MONTSERRAT_8
    { "MONTSERRAT_8", &lv_font_montserrat_8 },
#endif
#if LV_FONT_MONTSERRAT_10
    { "MONTSERRAT_10", &lv_font_montserrat_10 },
#endif
#if LV_FONT_MONTSERRAT_12
    { "MONTSERRAT_12", &lv_font_montserrat_12 },
#endif
#if LV_FONT_MONTSERRAT_14
    { "MONTSERRAT_14", &lv_font_montserrat_14 },
#endif
#if LV_FONT_MONTSERRAT_16
    { "MONTSERRAT_16", &lv_font_montserrat_16 },
#endif
#if LV_FONT_MONTSERRAT_18
    { "MONTSERRAT_18", &lv_font_montserrat_18 },
#endif
#if LV_FONT_MONTSERRAT_20
    { "MONTSERRAT_20", &lv_font_montserrat_20 },
#endif
#if LV_FONT_MONTSERRAT_22
    { "MONTSERRAT_22", &lv_font_montserrat_22 },
#endif
#if LV_FONT_MONTSERRAT_24
    { "MONTSERRAT_24", &lv_font_montserrat_24 },
#endif
#if LV_FONT_MONTSERRAT_26
    { "MONTSERRAT_26", &lv_font_montserrat_26 },
#endif
#if LV_FONT_MONTSERRAT_28
    { "MONTSERRAT_28", &lv_font_montserrat_28 },
#endif
#if LV_FONT_MONTSERRAT_30
    { "MONTSERRAT_30", &lv_font_montserrat_30 },
#endif
#if LV_FONT_MONTSERRAT_32
    { "MONTSERRAT_32", &lv_font_montserrat_32 },
#endif
#if LV_FONT_MONTSERRAT_34
    { "MONTSERRAT_34", &lv_font_montserrat_34 },
#endif
#if LV_FONT_MONTSERRAT_36
    { "MONTSERRAT_36", &lv_font_montserrat_36 },
#endif
#if LV_FONT_MONTSERRAT_38
    { "MONTSERRAT_38", &lv_font_montserrat_38 },
#endif
#if LV_FONT_MONTSERRAT_40
    { "MONTSERRAT_40", &lv_font_montserrat_40 },
#endif
#if LV_FONT_MONTSERRAT_42
    { "MONTSERRAT_42", &lv_font_montserrat_42 },
#endif
#if LV_FONT_MONTSERRAT_44
    { "MONTSERRAT_44", &lv_font_montserrat_44 },
#endif
#if LV_FONT_MONTSERRAT_46
    { "MONTSERRAT_46", &lv_font_montserrat_46 },
#endif
#if LV_FONT_MONTSERRAT_48
    { "MONTSERRAT_48", &lv_font_montserrat_48 },
#endif
};

//
//
//

void create_screens() {
    
    eez_flow_init_fonts(fonts, sizeof(fonts) / sizeof(ext_font_desc_t));

// Set default LVGL theme
    lv_display_t *dispp = lv_display_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), false, LV_FONT_DEFAULT);
    lv_display_set_theme(dispp, theme);
    
    // Initialize screens
    eez_flow_init_screen_names(screen_names, sizeof(screen_names) / sizeof(const char *));
    eez_flow_init_object_names(object_names, sizeof(object_names) / sizeof(const char *));
    
    // Create screens
    create_screen_page_lock();
    create_screen_page_main();
    create_screen_page_error();
}