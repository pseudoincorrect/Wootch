#include "start_screen.h"
#include "play_screen.h"
#include "gui_animation.h"
#include "gui_theme.h"
#include "wifi_screen.h"
#include "esp_log.h"


// STATIC PROTOTYPES
LV_EVENT_CB_DECLARE(start_button_cb);
LV_EVENT_CB_DECLARE(play_button_cb);

// EXTERN VARIABLES
LV_IMG_DECLARE(img_normal_dog);

// GLOBAL VARIABLES

// STATIC VARIABLES
static const char* TAG = "START";

/*******************************************************************************
 * @brief
 * @param
 */
void create_start_screen(void)
{
    gui_anim_out_all(lv_scr_act(), GUI_ANIM_FAST);
    gui_anim_bg(GUI_ANIM_FAST, GUI_ACCENT_BG_1, GUI_BG_NORMAL);

    lv_obj_t * title = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(title, "START YOUR WOOTCH !");
    lv_obj_set_style_local_text_color(
        title, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, GUI_ACCENT_FG_1);
    lv_obj_set_style_local_text_font(
        title, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_theme_get_font_title());
    lv_obj_align(title, NULL, LV_ALIGN_IN_TOP_MID, 0, 15);
    gui_anim_in(title, GUI_ANIM_SLOW);

    lv_obj_t * cont_start = lv_cont_create(lv_scr_act(), NULL);
    lv_obj_set_style_local_bg_color(
        cont_start, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, GUI_BG_2);
    lv_obj_set_width(cont_start, 300);
    lv_obj_set_height(cont_start, 180);
    lv_cont_set_layout(cont_start, LV_LAYOUT_ROW_MID);
    lv_obj_set_style_local_pad_all(
        cont_start, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 25);
    lv_obj_set_style_local_pad_inner(
        cont_start, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 10);
    lv_obj_align(cont_start, lv_scr_act(), LV_ALIGN_CENTER, 0, 10);

    lv_obj_t * img = lv_img_create(cont_start, NULL);
    lv_img_set_src(img, &img_normal_dog);
    lv_img_set_antialias(img, false);
    lv_obj_align(img, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_local_radius(img, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 10);
    lv_obj_set_style_local_clip_corner(
        img, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, true);

    lv_obj_t * cont_buttons = lv_cont_create(cont_start, NULL);
    lv_obj_set_style_local_bg_color(
        cont_buttons, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, GUI_BG_2);
    lv_obj_set_width(cont_buttons, 100);
    lv_obj_set_height(cont_buttons, 180);
    lv_cont_set_layout(cont_buttons, LV_LAYOUT_COLUMN_MID);
    lv_obj_set_style_local_pad_all(
        cont_buttons, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 25);
    lv_obj_set_style_local_pad_inner(
        cont_buttons, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 10);
    lv_obj_align(cont_buttons, cont_buttons, LV_ALIGN_CENTER, 0, 10);

    lv_obj_t* start_button = lv_btn_create(cont_buttons, NULL);
    lv_obj_align(start_button, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_width(start_button, 100);
    lv_obj_set_height(start_button, 50);
    lv_obj_t* start_label = lv_label_create(start_button, NULL);
    lv_label_set_text(start_label, "START");
    lv_obj_set_event_cb(start_button, start_button_cb);
    gui_anim_in(cont_buttons, GUI_ANIM_SLOW);

    lv_obj_t* play_button = lv_btn_create(cont_buttons, NULL);
    lv_obj_align(play_button, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_width(play_button, 100);
    lv_obj_set_height(play_button, 50);
    lv_obj_t* play_label = lv_label_create(play_button, NULL);
    lv_label_set_text(play_label, "PLAY");
    lv_obj_set_event_cb(play_button, play_button_cb);
    gui_anim_in(cont_buttons, GUI_ANIM_SLOW);
}

/*******************************************************************************
 * @brief
 * @param
 */
LV_EVENT_CB_DECLARE(start_button_cb)
{
    if(e == LV_EVENT_CLICKED)
    {
        create_wifi_screen();
    }
}

/*******************************************************************************
 * @brief
 * @param
 */
LV_EVENT_CB_DECLARE(play_button_cb)
{
    if(e == LV_EVENT_CLICKED)
    {
        create_play_screen();
    }
}
