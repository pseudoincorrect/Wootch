//#include "gui_screens.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "lvgl.h"
//#include "freertos/FreeRTOS.h"
//#include "freertos/queue.h"
//#include "project_extern_variables.h"
#include "gui_theme.h"
#include "gui_animation.h"
#include "start_screen.h"
#include "wifi_screen.h"

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
static const char * TAG = "SCREEN";

/*******************************************************************************
 * @brief
 * @param
 */
void init_screens(void)
{
    LV_THEME_DEFAULT_INIT(lv_theme_get_color_primary(),
                          lv_theme_get_color_secondary(),
                          LV_THEME_MATERIAL_FLAG_DARK,
                          lv_theme_get_font_small(), lv_theme_get_font_normal(),
                          lv_theme_get_font_subtitle(), lv_theme_get_font_title());

    lv_style_init(&style_box);
    lv_style_set_value_align(&style_box, LV_STATE_DEFAULT, LV_ALIGN_OUT_TOP_LEFT);
    lv_style_set_margin_top(&style_box, LV_STATE_DEFAULT, LV_DPX(5));

    bg_color_prev = GUI_BG_0;
    bg_color_act = GUI_BG_0;

    lv_theme_t * th = gui_theme_init(LV_COLOR_BLACK, LV_COLOR_BLACK,
                                     LV_THEME_MATERIAL_FLAG_DARK,
                                     lv_theme_get_font_small(), lv_theme_get_font_normal(),
                                     lv_theme_get_font_subtitle(), lv_theme_get_font_title());

    lv_theme_set_act(th);

    lv_obj_set_style_local_bg_color(lv_scr_act(), LV_OBJ_PART_MAIN,
                                    LV_STATE_DEFAULT,
                                    GUI_BG_0);

    bg_top = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_clean_style_list(bg_top, LV_OBJ_PART_MAIN);
    lv_obj_set_style_local_bg_opa(bg_top, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT,
                                  LV_OPA_COVER);
    lv_obj_set_style_local_bg_color(bg_top, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT,
                                    GUI_ACCENT_BG_1);
    lv_obj_set_size(bg_top, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_y(bg_top, GUI_BG_NORMAL);

    // create_start_screen();
    create_wifi_screen();
}

