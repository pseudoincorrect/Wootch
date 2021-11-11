#include "pairing_screen.h"
#include "gui_animation.h"
#include "gui_theme.h"
#include "watch_screen.h"
#include "pairing_screen.h"
#include "esp_log.h"
#include "app_state.h"

#define TITLE_BG_OVERFLOW (LV_VER_RES + GUI_BG_SMALL)

LV_EVENT_CB_DECLARE(slider_event_cb);
LV_EVENT_CB_DECLARE(return_icon_event_cb);

LV_IMG_DECLARE(img_sleepy_dog);
LV_IMG_DECLARE(img_normal_dog);
LV_IMG_DECLARE(img_watch_dog);
LV_IMG_DECLARE(icon_left_arrow);

static const char *TAG = "WATCH";
static lv_obj_t *page_watch;
static lv_obj_t *cont_dog;
static lv_obj_t *img_dog;

/**
 * @brief Create a watch screen object
 */
void create_watch_screen(void)
{
    gui_anim_out_all(lv_scr_act(), GUI_ANIM_FAST);
    gui_anim_bg(GUI_ANIM_FAST, GUI_ACCENT_BG_1, GUI_BG_NORMAL);

    lv_obj_t *title = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(title, "On Wootch !");
    lv_obj_set_style_local_text_color(
        title, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, GUI_ACCENT_FG_1);
    lv_obj_set_style_local_text_font(
        title, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_theme_get_font_title());
    lv_obj_align(title, NULL, LV_ALIGN_IN_TOP_MID, 0, 8);
    gui_anim_in(title, GUI_ANIM_SLOW);

    // Return Button/Icon
    lv_obj_t *btn_ret = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_event_cb(btn_ret, return_icon_event_cb);
    // lv_btn_set_fit2(btn_ret, LV_FIT_NONE, LV_FIT_TIGHT);
    lv_obj_set_width(btn_ret, 40);
    lv_obj_set_height(btn_ret, 40);
    lv_obj_set_click(btn_ret, true);
    gui_anim_in(btn_ret, GUI_ANIM_SLOW);
    lv_obj_align(btn_ret, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    lv_obj_set_style_local_bg_color(
        btn_ret, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, GUI_ACCENT_BG_1);
    lv_obj_t *img_left = lv_img_create(btn_ret, NULL);
    lv_img_set_src(img_left, &icon_left_arrow);
    lv_img_set_antialias(img_left, false);
    lv_obj_set_width(img_left, icon_left_arrow.header.w);
    lv_obj_set_height(img_left, icon_left_arrow.header.h);
    lv_obj_align(img_left, NULL, LV_ALIGN_CENTER, 0, 0);

    // Main container
    lv_obj_t *cont_watch = lv_cont_create(lv_scr_act(), NULL);
    lv_obj_set_style_local_bg_color(
        cont_watch, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, GUI_BG_2);
    lv_obj_set_width(cont_watch, 300);
    lv_obj_set_height(cont_watch, 200);
    lv_obj_align(cont_watch, NULL, LV_ALIGN_CENTER, 0, 10);
    lv_cont_set_layout(cont_watch, LV_LAYOUT_CENTER);
    lv_obj_set_style_local_pad_all(
        cont_watch, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, -10);
    lv_obj_set_style_local_pad_inner(
        cont_watch, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 18);
    gui_anim_in(cont_watch, GUI_ANIM_SLOW);

    // Image
    img_dog = lv_img_create(cont_watch, NULL);
    lv_img_set_src(img_dog, &img_sleepy_dog);
    lv_img_set_antialias(img_dog, false);
    lv_obj_set_width(img_dog, 145);
    lv_obj_set_height(img_dog, 145);
    lv_obj_set_style_local_radius(img_dog, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 10);
    lv_obj_set_style_local_clip_corner(
        img_dog, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, true);

    // Slider
    lv_obj_t *slider = lv_slider_create(cont_watch, NULL);
    lv_slider_set_value(slider, 0, LV_ANIM_OFF);
    lv_obj_set_width(slider, 250);
    lv_obj_set_event_cb(slider, slider_event_cb);
    lv_obj_set_style_local_margin_top(
        slider, LV_SLIDER_PART_BG, LV_STATE_DEFAULT, LV_DPX(25));
}

/**
 * @brief Construct a new lv Slider event cb declare object
 */
LV_EVENT_CB_DECLARE(slider_event_cb)
{
    if (e == LV_EVENT_VALUE_CHANGED)
    {
        if (lv_slider_get_type(obj) == LV_SLIDER_TYPE_NORMAL)
        {
            int16_t val = lv_slider_get_value(obj);
            if (val < 33)
            {
                lv_img_set_src(img_dog, &img_sleepy_dog);
                app_state_set_security_lvl(SECU_LVL_1);
            }
            else if (val < 67)
            {
                lv_img_set_src(img_dog, &img_normal_dog);
                app_state_set_security_lvl(SECU_LVL_2);
            }
            else
            {
                lv_img_set_src(img_dog, &img_watch_dog);
                app_state_set_security_lvl(SECU_LVL_1);
            }
        }
        lv_obj_set_width(img_dog, 145);
        lv_obj_set_height(img_dog, 145);
        lv_obj_set_style_local_radius(img_dog, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 10);
        lv_obj_set_style_local_clip_corner(
            img_dog, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, true);
    }
}

/**
 * @brief Construct a new lv Icon event cb declare object
 */
LV_EVENT_CB_DECLARE(return_icon_event_cb)
{
    if (e == LV_EVENT_CLICKED)
    {
        create_pairing_screen();
    }
}
