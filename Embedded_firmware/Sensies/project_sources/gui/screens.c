
#include "lvgl.h"

static void controls_create(lv_obj_t* tabvie);


static lv_obj_t * tv;
static lv_obj_t * t1;
static lv_style_t style_box;

void sensie_gui(void)
{
    tv = lv_tabview_create(lv_scr_act(), NULL);
    t1 = lv_tabview_add_tab(tv, "Controls");

    lv_style_init(&style_box);
    lv_style_set_value_align(&style_box, LV_STATE_DEFAULT, LV_ALIGN_OUT_TOP_LEFT);
    lv_style_set_value_ofs_y(&style_box, LV_STATE_DEFAULT, - LV_DPX(10));
    lv_style_set_margin_top(&style_box, LV_STATE_DEFAULT, LV_DPX(30));

    controls_create(t1);
}

static void controls_create(lv_obj_t* parent)
{
    lv_page_set_scrl_layout(parent, LV_LAYOUT_PRETTY_TOP);

    lv_disp_size_t disp_size = lv_disp_get_size_category(NULL);
    lv_coord_t grid_w = lv_page_get_width_grid(parent,
                        disp_size <= LV_DISP_SIZE_SMALL ? 1 : 2, 1);


    lv_obj_t * h = lv_cont_create(parent, NULL);
    lv_cont_set_layout(h, LV_LAYOUT_PRETTY_MID);
    lv_obj_add_style(h, LV_CONT_PART_MAIN, &style_box);
    lv_obj_set_drag_parent(h, true);

    lv_obj_set_style_local_value_str(h, LV_CONT_PART_MAIN, LV_STATE_DEFAULT,
                                     "Basics");

    lv_cont_set_fit2(h, LV_FIT_NONE, LV_FIT_TIGHT);
    lv_obj_set_width(h, grid_w);
    lv_obj_t * btn = lv_btn_create(h, NULL);
    lv_btn_set_fit2(btn, LV_FIT_NONE, LV_FIT_TIGHT);
    lv_obj_set_width(btn, lv_obj_get_width_grid(h,
                     disp_size <= LV_DISP_SIZE_SMALL ? 1 : 2, 1));
    lv_obj_t * label = lv_label_create(btn, NULL);
    lv_label_set_text(label, "Button");


    btn = lv_btn_create(h, btn);
    lv_btn_toggle(btn);
    label = lv_label_create(btn, NULL);
    lv_label_set_text(label, "Button");

}

