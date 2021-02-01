#include "watch_screen.h"
#include "gui_animation.h"
#include "gui_theme.h"
#include "start_screen.h"

/**********************
 *  STATIC PROTOTYPES
 **********************/
LV_EVENT_CB_DECLARE(watch_button_cb);

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
void create_watch_screen(void)
{
    gui_anim_out_all(lv_scr_act(), GUI_ANIM_FAST);

    gui_anim_bg(GUI_ANIM_FAST, GUI_GREEN, GUI_BG_FULL);

    LV_IMG_DECLARE(imp_normal_dog);

    lv_obj_t * btn = lv_btn_create(lv_scr_act(), NULL);
    lv_theme_apply(btn, (lv_theme_style_t)GUI_THEME_BTN_BORDER);
    lv_obj_set_size(btn, GUI_BTN_W, GUI_BTN_H);
    lv_obj_align(btn, NULL, LV_ALIGN_CENTER, 0, 50);
    lv_obj_set_style_local_value_str(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT,
                                     "CONNECT");
    lv_obj_set_event_cb(btn, watch_button_cb);

    gui_anim_in(btn, GUI_ANIM_SLOW);
}

/*******************************************************************************
 * @brief
 * @param
 */
LV_EVENT_CB_DECLARE(watch_button_cb)
{
    create_start_screen();
}