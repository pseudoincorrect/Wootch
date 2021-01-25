#include "wifi_screen.h"
#include "gui_animation.h"
#include "gui_theme.h"
#include "watch_screen.h"

/**********************
 *  STATIC PROTOTYPES
 **********************/
LV_EVENT_CB_DECLARE(connect_wifi_button_cb);

/**********************
 *  GLOBAL VARIABLES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/*******************************************************************************
 * @brief
 * @param
 */
void create_wifi_screen(uint32_t delay)
{
    gui_anim_out_all(lv_scr_act(), 0);

    gui_anim_bg(0, GUI_GREEN, GUI_BG_FULL);

    LV_IMG_DECLARE(imp_normal_dog);

    lv_obj_t * btn = lv_btn_create(lv_scr_act(), NULL);
    lv_theme_apply(btn, (lv_theme_style_t)GUI_THEME_BTN_BORDER);
    lv_obj_set_size(btn, GUI_BTN_W, GUI_BTN_H);
    lv_obj_align(btn, NULL, LV_ALIGN_CENTER, 0, 50);
    lv_obj_set_style_local_value_str(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT,
                                     "CONNECT");
    lv_obj_set_event_cb(btn, connect_wifi_button_cb);

    gui_anim_in(btn, delay);
    delay += GUI_ANIM_DELAY;
    gui_anim_in(btn, delay);
}

/*******************************************************************************
 * @brief
 * @param
 */
LV_EVENT_CB_DECLARE(connect_wifi_button_cb)
{
    create_watch_screen(200);
}