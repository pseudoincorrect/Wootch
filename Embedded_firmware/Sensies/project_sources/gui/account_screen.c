#include "account_screen.h"
#include "gui_animation.h"
#include "gui_theme.h"
#include "watch_screen.h"
#include "wifi_screen.h"
#include "esp_log.h"

#define TITLE_BG_OVERFLOW (LV_VER_RES + GUI_BG_SMALL)

// STATIC PROTOTYPES
LV_EVENT_CB_DECLARE(btn_connect_cb);
LV_EVENT_CB_DECLARE(ta_event_cb);
LV_EVENT_CB_DECLARE(kb_event_cb);
LV_EVENT_CB_DECLARE(return_icon_event_cb);

// EXTERN VARIABLES
LV_IMG_DECLARE(icon_left_arrow);

// GLOBAL VARIABLES

// STATIC VARIABLES
static const char *TAG = "ACCOUNT";
static char account_login[32] = {0};
static char account_passw[32] = {0};
static lv_obj_t * kb;
static lv_obj_t * ta_login;
static lv_obj_t * ta_passw;
static lv_obj_t * page_account;

/*******************************************************************************
 * @brief
 * @param
 */
void create_account_screen(void)
{
    // Background
    gui_anim_out_all(lv_scr_act(), GUI_ANIM_FAST);
    gui_anim_bg(GUI_ANIM_FAST, GUI_ACCENT_BG_1, GUI_BG_SMALL);

    // Title
    lv_obj_t * title = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(title, "Connect to your Wootch Account");
    lv_obj_set_style_local_text_color(
        title, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, GUI_ACCENT_FG_1);
    lv_obj_set_style_local_text_font(
        title, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_theme_get_font_title());
    lv_obj_align(title, NULL, LV_ALIGN_IN_TOP_MID, 0, 13);
    gui_anim_in(title, GUI_ANIM_SLOW);

    // Return Icon
    lv_obj_t * img_left = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(img_left, &icon_left_arrow);
    lv_img_set_antialias(img_left, false);
    lv_obj_set_width(img_left, icon_left_arrow.header.w);
    lv_obj_set_height(img_left, icon_left_arrow.header.h);
    lv_obj_align(img_left, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 5);
    lv_obj_set_event_cb(img_left, return_icon_event_cb);
    lv_obj_set_click(img_left, true);
    gui_anim_in(img_left, GUI_ANIM_SLOW);

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

    // SSID
    // Container SSID
    lv_obj_t * cont_login = lv_cont_create(page_account, NULL);
    lv_cont_set_layout(cont_login, LV_LAYOUT_CENTER);
    lv_obj_set_drag_parent(cont_login, true);
    // lv_cont_set_fit(cont_login, LV_FIT_NONE);
    lv_obj_set_height(cont_login, 40);
    lv_obj_set_width(cont_login, 200);
    lv_theme_apply(cont_login, (lv_theme_style_t)GUI_THEME_BOX_BORDER);
    // Text area SSID
    ta_login = lv_textarea_create(cont_login, NULL);
    lv_obj_set_width(ta_login, 180);
    lv_textarea_set_text(ta_login, "");
    lv_textarea_set_placeholder_text(ta_login, "Wootch Account Email");
    lv_textarea_set_one_line(ta_login, true);
    lv_textarea_set_cursor_hidden(ta_login, true);
    lv_theme_apply(ta_login, (lv_theme_style_t)LV_THEME_CONT);
    lv_textarea_set_text_align(ta_login, LV_LABEL_ALIGN_CENTER);
    lv_obj_set_event_cb(ta_login, ta_event_cb);
    // lv_textarea_set_text(ta_login, "Maxi"); // please hack me (^_^')

    // PASSWORD
    // Container PASSWORD
    lv_obj_t * cont_passwd = lv_cont_create(page_account, NULL);
    lv_cont_set_layout(cont_passwd, LV_LAYOUT_CENTER);
    lv_obj_set_drag_parent(cont_passwd, true);
    // lv_cont_set_fit(cont_passwd, LV_FIT_NONE);
    lv_obj_set_height(cont_passwd, 40);
    lv_obj_set_width(cont_passwd, 200);
    lv_theme_apply(cont_passwd, (lv_theme_style_t)GUI_THEME_BOX_BORDER);
    // Text area SSID PASSWORD
    ta_passw = lv_textarea_create(cont_passwd, NULL);
    lv_obj_set_width(ta_passw, 180);
    lv_textarea_set_text(ta_passw, "");
    lv_textarea_set_placeholder_text(ta_passw, "Wootch Account Password");
    lv_textarea_set_one_line(ta_passw, true);
    lv_textarea_set_cursor_hidden(ta_passw, true);
    lv_theme_apply(ta_passw, (lv_theme_style_t)LV_THEME_CONT);
    lv_textarea_set_text_align(ta_passw, LV_LABEL_ALIGN_CENTER);
    lv_obj_set_event_cb(ta_passw, ta_event_cb);
    // lv_textarea_set_text(ta_passw, "notmyrealpassword"); // please hack me (^_^')

    // Connect Button
    lv_obj_t * btn_conn = lv_btn_create(page_account, NULL);
    lv_obj_set_event_cb(btn_conn, btn_connect_cb);
    // lv_btn_set_fit2(btn_conn, LV_FIT_NONE, LV_FIT_TIGHT);
    lv_obj_set_width(btn_conn, 100);
    lv_obj_set_height(btn_conn, 40);
    lv_obj_t * lb_conn = lv_label_create(btn_conn, NULL);
    lv_label_set_text(lb_conn, "Connect");
    lv_obj_set_drag_parent(btn_conn, true);
    lv_btn_toggle(btn_conn);
}

/*******************************************************************************
 * @brief
 * @param
 */
LV_EVENT_CB_DECLARE(btn_connect_cb)
{
    if(e == LV_EVENT_CLICKED)
    {
        const char* login = lv_textarea_get_text(ta_login);
        const char* pass = lv_textarea_get_text(ta_passw);
        // screen_cb.account_connect((char*) login, (char*) pass);
        create_watch_screen();
    }
}

/*******************************************************************************
 * @brief
 * @param
 */
LV_EVENT_CB_DECLARE(ta_event_cb)
{
    if(e == LV_EVENT_RELEASED)
    {
        if(kb == NULL)
        {
            lv_coord_t h = lv_obj_get_height(page_account);
            lv_obj_set_height(page_account, LV_VER_RES / 2 - TITLE_BG_OVERFLOW);
            kb = lv_keyboard_create(lv_scr_act(), NULL);
            lv_obj_set_event_cb(kb, kb_event_cb);
        }
        lv_textarea_set_cursor_hidden(obj, false);
        lv_keyboard_set_textarea(kb, obj);
    }
    else if(e == LV_EVENT_DEFOCUSED)
    {
        lv_textarea_set_cursor_hidden(obj, true);
    }
}

/*******************************************************************************
 * @brief
 * @param
 */
LV_EVENT_CB_DECLARE(kb_event_cb)
{
    lv_keyboard_def_event_cb(kb, e);

    if(e == LV_EVENT_CANCEL || e == LV_EVENT_APPLY)
    {
        if(kb)
        {
            lv_coord_t h = lv_obj_get_height(page_account);
            lv_obj_set_height(page_account, -GUI_BG_SMALL);
            lv_obj_del(kb);
            kb = NULL;
        }
    }
}

/*******************************************************************************
 * @brief
 * @param
 */
LV_EVENT_CB_DECLARE(return_icon_event_cb)
{
    if(e == LV_EVENT_CLICKED)
    {
        create_wifi_screen();
    }
}


