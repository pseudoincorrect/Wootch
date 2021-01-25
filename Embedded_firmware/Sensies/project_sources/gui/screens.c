#include "gui_screens.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "lvgl.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "project_extern_variables.h"
#include "gui_theme.h"
#include "gui_animation.h"
#include "start_screen.h"

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void home_open(void);
static void create_home(uint32_t delay);
static void create_settings(void);
LV_EVENT_CB_DECLARE(setup_icon_event_cb);
LV_EVENT_CB_DECLARE(back_to_home_event_cb);
LV_EVENT_CB_DECLARE(icon_generic_event_cb);
LV_EVENT_CB_DECLARE(home_setup_button_cb);

/**********************
 *  GLOBAL VARIABLES
 **********************/
lv_style_t style_box;
lv_obj_t * scan_img;
lv_obj_t * bg_top;
lv_obj_t * bg_bottom;
lv_color_t bg_color_prev;
lv_color_t bg_color_act;

/**********************
 *  STATIC VARIABLES
 **********************/


/*******************************************************************************
 * @brief
 * @param
 */
void init_screens(void)
{
    // LV_THEME_DEFAULT_INIT(lv_theme_get_color_primary(),
    //                       lv_theme_get_color_secondary(),
    //                       LV_THEME_MATERIAL_FLAG_DARK,
    //                       lv_theme_get_font_small(), lv_theme_get_font_normal(),
    //                       lv_theme_get_font_subtitle(), lv_theme_get_font_title());

    // lv_style_init(&style_box);
    // lv_style_set_value_align(&style_box, LV_STATE_DEFAULT, LV_ALIGN_OUT_TOP_LEFT);
    // lv_style_set_margin_top(&style_box, LV_STATE_DEFAULT, LV_DPX(5));

    bg_color_prev = GUI_BLUE;
    bg_color_act = GUI_BLUE;

    lv_theme_t * th = gui_theme_init(LV_COLOR_BLACK, LV_COLOR_BLACK, 0,
                                     lv_theme_get_font_small(), lv_theme_get_font_normal(),
                                     lv_theme_get_font_subtitle(), lv_theme_get_font_title());

    lv_theme_set_act(th);

    lv_obj_t * scr = lv_obj_create(NULL, NULL);
    lv_scr_load(scr);

    bg_top = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_clean_style_list(bg_top, LV_OBJ_PART_MAIN);
    lv_obj_set_style_local_bg_opa(bg_top, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT,
                                  LV_OPA_COVER);
    lv_obj_set_style_local_bg_color(bg_top, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT,
                                    GUI_BLUE);
    lv_obj_set_size(bg_top, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_y(bg_top, GUI_BG_NORMAL);

    create_start_screen(0);
}


/*******************************************************************************
 * @brief
 * @param
 */
static void home_open(void)
{
    uint32_t delay = 200;

    LV_IMG_DECLARE(icon_wifi);
    LV_IMG_DECLARE(img_sleepy_dog);
    LV_IMG_DECLARE(img_normal_dog);
    LV_IMG_DECLARE(img_play_dog);
    LV_IMG_DECLARE(img_watch_dog);


    lv_obj_t * icon;
    lv_obj_t * cont = lv_cont_create(lv_scr_act(), NULL);
    lv_obj_set_size(cont, 350, 80);
    lv_obj_clean_style_list(cont, LV_CONT_PART_MAIN);
    lv_obj_align(cont, NULL, LV_ALIGN_IN_TOP_LEFT, 60, 0);

    // icon = lv_img_create(cont, NULL);
    // lv_img_set_src(icon, &img_sleepy_dog);
    // lv_obj_align_origo(icon, NULL, LV_ALIGN_IN_TOP_LEFT, 20, 50);
    // // gui_anim_in(icon, delay);

    // lv_obj_t* cont_dog = lv_cont_create(lv_scr_act(), NULL);
    // lv_cont_set_layout(cont_dog, LV_LAYOUT_CENTER);
    // lv_obj_add_style(cont_dog, LV_CONT_PART_MAIN, &style_box);
    // lv_obj_set_drag_parent(cont_dog, true);
    // lv_cont_set_fit2(cont_dog, LV_FIT_TIGHT, LV_FIT_TIGHT);

    // LV_IMG_DECLARE(img_sleepy_dog);
    // lv_obj_t* img_dog = lv_img_create(cont_dog, NULL);
    // lv_img_set_src(img_dog, &img_sleepy_dog);
    // lv_obj_set_width(img_dog, img_sleepy_dog.header.w);
    // lv_obj_set_height(img_dog, img_sleepy_dog.header.h);
    // lv_img_set_auto_size(img_dog, true);

    // icon = lv_img_create(cont, NULL);
    // lv_img_set_src(icon, &img_normal_dog);
    // lv_obj_align_origo(icon, NULL, LV_ALIGN_IN_TOP_LEFT, 110, 50);
    // // gui_anim_in(icon, delay);

    // icon = lv_img_create(cont, NULL);
    // lv_img_set_src(icon, &img_play_dog);
    // lv_obj_align_origo(icon, NULL, LV_ALIGN_IN_TOP_LEFT, 200, 50);
    // // gui_anim_in(icon, delay);

    icon = lv_img_create(cont, NULL);
    lv_img_set_src(icon, &img_watch_dog);
    lv_obj_align_origo(icon, NULL, LV_ALIGN_IN_TOP_LEFT, 200, 50);

    lv_obj_t * btn_next_page = lv_btn_create(cont, NULL);
    lv_obj_set_event_cb(btn_next_page, setup_icon_event_cb);
    lv_btn_set_fit2(btn_next_page, LV_FIT_NONE, LV_FIT_TIGHT);
    lv_obj_set_width(btn_next_page, 100);
    lv_obj_t * lb_next_page = lv_label_create(btn_next_page, NULL);
    lv_label_set_text(lb_next_page, "page");
    lv_obj_align_origo(btn_next_page, NULL, LV_ALIGN_IN_TOP_LEFT, 290, 50);

    // lv_btn_toggle(btn_conn);
    // gui_anim_in(icon, delay);

    // lv_obj_t * title = add_title("22 April 2020 15:36");
    // lv_obj_align(title, NULL, LV_ALIGN_IN_TOP_RIGHT, -60, 35);

}

/*******************************************************************************
 * @brief
 * @param
 */
static void info_bottom_create(const char * dsc, const char * btn_txt,
                               lv_event_cb_t btn_event_cb, uint32_t delay)
{

    lv_obj_t * txt = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(txt, dsc);
    lv_theme_apply(txt, (lv_theme_style_t)GUI_THEME_LABEL_WHITE);
    lv_obj_align(txt, NULL, LV_ALIGN_CENTER, 0, 50);

    lv_obj_t * btn = lv_btn_create(lv_scr_act(), NULL);
    lv_theme_apply(btn, (lv_theme_style_t)GUI_THEME_BTN_BORDER);
    lv_obj_set_size(btn, GUI_BTN_W, GUI_BTN_H);
    lv_obj_align(btn, txt, LV_ALIGN_OUT_BOTTOM_MID, 0, 30);
    lv_obj_set_style_local_value_str(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT,
                                     btn_txt);
    lv_obj_set_event_cb(btn, btn_event_cb);

    gui_anim_in(txt, delay);
    delay += GUI_ANIM_DELAY;

    gui_anim_in(btn, delay);
    delay += GUI_ANIM_DELAY;
    gui_anim_in(btn, delay);
}

/*******************************************************************************
 *******************************************************************************
 * @brief
 * @param
 */
static void create_home(uint32_t delay)
{
    gui_anim_out_all(lv_scr_act(), 0);

    gui_anim_bg(0, GUI_GREEN, GUI_BG_FULL);

    // uint32_t delay = 200;

    LV_IMG_DECLARE(imp_normal_dog);

    lv_obj_t * btn = lv_btn_create(lv_scr_act(), NULL);
    lv_theme_apply(btn, (lv_theme_style_t)GUI_THEME_BTN_BORDER);
    lv_obj_set_size(btn, GUI_BTN_W, GUI_BTN_H);
    lv_obj_align(btn, NULL, LV_ALIGN_CENTER, 0, 50);
    lv_obj_set_style_local_value_str(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT,
                                     "START");
    lv_obj_set_event_cb(btn, home_setup_button_cb);
    // lv_obj_set_event_cb(btn, setup_icon_event_cb);

    gui_anim_in(btn, delay);
    delay += GUI_ANIM_DELAY;
    gui_anim_in(btn, delay);
}

/*******************************************************************************
 * @brief
 * @param
 */
LV_EVENT_CB_DECLARE(home_setup_button_cb)
{
    create_settings();
}


/*******************************************************************************
 * @brief
 * @param
 */
static void create_settings(void)
{
    gui_anim_out_all(lv_scr_act(), 0);

    gui_anim_bg(0, GUI_RED, GUI_BG_FULL);

    uint32_t delay = 200;

    LV_IMG_DECLARE(icon_wifi);

    lv_obj_t * img = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(img, &icon_wifi);
    lv_obj_align(img, NULL, LV_ALIGN_CENTER, -90, 0);

    gui_anim_in(img, delay);
    delay += GUI_ANIM_DELAY;

    // LV_IMG_DECLARE(gui_img_no_internet);
    img = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(img, &icon_wifi);
    lv_obj_align(img, NULL, LV_ALIGN_CENTER, 0, -40);

    gui_anim_in(img, delay);
    delay += GUI_ANIM_DELAY;

    // LV_IMG_DECLARE(gui_img_cloud);
    img = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(img, &icon_wifi);
    lv_obj_align(img, NULL, LV_ALIGN_CENTER, 80, -80);

    gui_anim_in(img, delay);
    delay += GUI_ANIM_DELAY;

    info_bottom_create("You have no permission to change the settings.", "BACK",
                       back_to_home_event_cb, delay);
}

/*******************************************************************************
 * @brief
 * @param
 */
LV_EVENT_CB_DECLARE(setup_icon_event_cb)
{
    if (e == LV_EVENT_CLICKED)
    {
        create_home(200);
    }

    icon_generic_event_cb(obj, e);
}

/*******************************************************************************
 * @brief
 * @param
 */
LV_EVENT_CB_DECLARE(back_to_home_event_cb)
{
    if (e == LV_EVENT_CLICKED)
    {
        // scan_img = NULL;
        gui_anim_out_all(lv_scr_act(), 0);
        home_open();
    }
}

/*******************************************************************************
 * @brief
 * @param
 */
LV_EVENT_CB_DECLARE(icon_generic_event_cb)
{
    if (e == LV_EVENT_PRESSED)
    {
        lv_obj_t * img = lv_obj_get_child_back(obj, NULL);
        lv_obj_t * txt = lv_obj_get_child(obj, NULL);

        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_time(&a, 100);

        lv_anim_set_var(&a, img);
        lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_x);
        lv_anim_set_values(&a, lv_obj_get_x(img),
                           lv_obj_get_width(obj) - lv_obj_get_width(img) - 35);
        lv_anim_start(&a);

        lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
        lv_anim_set_values(&a, lv_obj_get_y(img), 35);
        lv_anim_start(&a);

        lv_anim_set_var(&a, txt);
        lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_x);
        lv_anim_set_values(&a, lv_obj_get_x(txt), 35);
        lv_anim_start(&a);

        lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
        lv_anim_set_values(&a, lv_obj_get_y(txt),
                           lv_obj_get_height(obj) - lv_obj_get_height(txt) - 35);
        lv_anim_start(&a);
    }
    else if(e == LV_EVENT_PRESS_LOST || e == LV_EVENT_RELEASED)
    {
        lv_obj_t * img = lv_obj_get_child_back(obj, NULL);
        lv_obj_t * txt = lv_obj_get_child(obj, NULL);
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_time(&a, 100);
        lv_anim_set_var(&a, img);

        lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_x);
        lv_anim_set_values(&a, lv_obj_get_x(img),
                           lv_obj_get_width(obj) - lv_obj_get_width(img) - 30);
        lv_anim_start(&a);

        lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
        lv_anim_set_values(&a, lv_obj_get_y(img), 30);
        lv_anim_start(&a);

        lv_anim_set_var(&a, txt);
        lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_x);
        lv_anim_set_values(&a, lv_obj_get_x(txt), 30);
        lv_anim_start(&a);

        lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
        lv_anim_set_values(&a, lv_obj_get_y(txt),
                           lv_obj_get_height(obj) - lv_obj_get_height(txt) - 30);
        lv_anim_start(&a);
    }
}