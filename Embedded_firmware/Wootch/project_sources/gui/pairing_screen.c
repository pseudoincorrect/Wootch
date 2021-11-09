#include "pairing_screen.h"
#include "gui_animation.h"
#include "gui_theme.h"
#include "watch_screen.h"
#include "wifi_screen.h"
#include "esp_log.h"

#define TITLE_BG_OVERFLOW (LV_VER_RES + GUI_BG_SMALL)

LV_EVENT_CB_DECLARE(btn_pair_cb);
LV_EVENT_CB_DECLARE(btn_unpair_cb);
LV_EVENT_CB_DECLARE(btn_continue_cb);
LV_EVENT_CB_DECLARE(return_icon_event_cb);

LV_IMG_DECLARE(icon_left_arrow);

static const char *TAG = "ACCOUNT";
static char account_login[32] = {0};
static char account_passw[32] = {0};
static lv_obj_t *kb;
static lv_obj_t *ta_pairing;
static lv_obj_t *ta_user;
static lv_obj_t *page_account;
static lv_obj_t *cont_pairing;
static lv_obj_t *cont_user;

/**
 * @brief Create a pairing screen object
 */
void create_pairing_screen(void)
{
    // Background
    gui_anim_out_all(lv_scr_act(), GUI_ANIM_FAST);
    gui_anim_bg(GUI_ANIM_FAST, GUI_ACCENT_BG_1, GUI_BG_SMALL);

    // Title
    lv_obj_t *title = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(title, "Wootch Account");
    lv_obj_set_style_local_text_color(
        title, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, GUI_ACCENT_FG_1);
    lv_obj_set_style_local_text_font(
        title, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_theme_get_font_title());
    lv_obj_align(title, NULL, LV_ALIGN_IN_TOP_MID, 0, 13);
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
    page_account = lv_page_create(lv_scr_act(), NULL);
    lv_obj_set_pos(page_account, 0, TITLE_BG_OVERFLOW);
    lv_obj_set_width(page_account, LV_HOR_RES_MAX);
    lv_obj_set_height(page_account, -GUI_BG_SMALL);
    lv_page_set_scrl_layout(page_account, LV_LAYOUT_CENTER);
    lv_obj_set_style_local_pad_all(
        page_account, LV_PAGE_PART_SCROLLABLE, LV_STATE_DEFAULT, 10);
    lv_obj_set_style_local_pad_inner(
        page_account, LV_PAGE_PART_SCROLLABLE, LV_STATE_DEFAULT, 10);
    gui_anim_in(page_account, GUI_ANIM_SLOW);

    // PAIRING
    // Container PAIRING
    cont_pairing = lv_cont_create(page_account, NULL);
    lv_cont_set_layout(cont_pairing, LV_LAYOUT_CENTER);
    lv_obj_set_drag_parent(cont_pairing, true);
    lv_obj_set_height(cont_pairing, 40);
    lv_obj_set_width(cont_pairing, 200);
    lv_theme_apply(cont_pairing, (lv_theme_style_t)GUI_THEME_BOX_BORDER);
    // Text area PAIRING
    ta_pairing = lv_textarea_create(cont_pairing, NULL);
    lv_obj_set_width(ta_pairing, 180);
    lv_textarea_set_text(ta_pairing, "");
    lv_textarea_set_one_line(ta_pairing, true);
    lv_textarea_set_cursor_hidden(ta_pairing, true);
    lv_theme_apply(ta_pairing, (lv_theme_style_t)LV_THEME_CONT);
    lv_textarea_set_text_align(ta_pairing, LV_LABEL_ALIGN_CENTER);
    lv_textarea_set_text(ta_pairing, " ");

    // USER
    // Container USER
    cont_user = lv_cont_create(page_account, NULL);
    lv_cont_set_layout(cont_user, LV_LAYOUT_CENTER);
    lv_obj_set_drag_parent(cont_user, true);
    lv_obj_set_height(cont_user, 40);
    lv_obj_set_width(cont_user, 200);
    lv_theme_apply(cont_user, (lv_theme_style_t)GUI_THEME_BOX_BORDER);
    // Text area USER
    ta_user = lv_textarea_create(cont_user, NULL);
    lv_obj_set_width(ta_user, 180);
    lv_textarea_set_text(ta_user, "");
    lv_textarea_set_one_line(ta_user, true);
    lv_theme_apply(ta_user, (lv_theme_style_t)LV_THEME_CONT);
    lv_textarea_set_text_align(ta_user, LV_LABEL_ALIGN_CENTER);
    lv_textarea_set_text(ta_user, "No user paired");
    lv_obj_set_hidden(cont_pairing, true);

    lv_obj_t *cont_buttons = lv_cont_create(page_account, NULL);
    lv_obj_set_width(cont_buttons, 300);
    lv_obj_set_height(cont_buttons, 70);
    lv_obj_set_style_local_pad_top(
        cont_buttons, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 25);
    lv_cont_set_layout(cont_buttons, LV_LAYOUT_PRETTY_MID);

    // Pairing Button
    lv_obj_t *btn_pair = lv_btn_create(cont_buttons, NULL);
    lv_obj_set_event_cb(btn_pair, btn_pair_cb);
    lv_obj_set_width(btn_pair, 90);
    lv_obj_set_height(btn_pair, 40);
    lv_obj_set_drag_parent(btn_pair, true);
    lv_btn_toggle(btn_pair);
    lv_obj_t *lb_pair = lv_label_create(btn_pair, NULL);
    lv_label_set_text(lb_pair, "Pair");

    // Unpair Button
    lv_obj_t *btn_unpair = lv_btn_create(cont_buttons, NULL);
    lv_obj_set_event_cb(btn_unpair, btn_unpair_cb);
    lv_obj_set_width(btn_unpair, 90);
    lv_obj_set_height(btn_unpair, 40);
    lv_obj_set_drag_parent(btn_unpair, true);
    lv_btn_toggle(btn_unpair);
    lv_obj_t *lb_unpair = lv_label_create(btn_unpair, NULL);
    lv_label_set_text(lb_unpair, "Unpair");

    // Continue Button
    lv_obj_t *btn_continue = lv_btn_create(cont_buttons, NULL);
    lv_obj_set_event_cb(btn_continue, btn_continue_cb);
    lv_obj_set_width(btn_continue, 90);
    lv_obj_set_height(btn_continue, 40);
    lv_obj_set_drag_parent(btn_continue, true);
    lv_btn_toggle(btn_continue);
    lv_obj_t *lb_continue = lv_label_create(btn_continue, NULL);
    lv_label_set_text(lb_continue, "Continue");
}

/**
 * @brief Construct a new lv Button event cb declare object
 */
LV_EVENT_CB_DECLARE(btn_pair_cb)
{
    if (e == LV_EVENT_CLICKED)
    {
        lv_obj_set_hidden(cont_pairing, false);
        lv_obj_set_hidden(cont_user, true);
    }
}

/**
 * @brief Construct a new lv Button event cb declare object
 */
LV_EVENT_CB_DECLARE(btn_unpair_cb)
{
    if (e == LV_EVENT_CLICKED)
    {
        lv_obj_set_hidden(cont_pairing, true);
        lv_obj_set_hidden(cont_user, false);
    }
}

/**
 * @brief Construct a new lv Button event cb declare object
 */
LV_EVENT_CB_DECLARE(btn_continue_cb)
{
    if (e == LV_EVENT_CLICKED)
    {
        create_watch_screen();
    }
}

/**
 * @brief Construct a new lv Icon event cb declare object
 */
LV_EVENT_CB_DECLARE(return_icon_event_cb)
{
    if (e == LV_EVENT_CLICKED)
    {
        create_wifi_screen();
    }
}
