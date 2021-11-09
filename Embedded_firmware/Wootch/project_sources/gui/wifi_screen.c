#include "wifi_screen.h"
#include "gui_animation.h"
#include "gui_theme.h"
#include "start_screen.h"
#include "pairing_screen.h"
#include "esp_log.h"
#include "esp_err.h"
#include "app_state.h"
#include "app_nvs.h"

#define TITLE_BG_OVERFLOW (LV_VER_RES + GUI_BG_SMALL)

LV_EVENT_CB_DECLARE(btn_connect_cb);
LV_EVENT_CB_DECLARE(ta_event_cb);
LV_EVENT_CB_DECLARE(kb_event_cb);
LV_EVENT_CB_DECLARE(return_icon_event_cb);

LV_IMG_DECLARE(icon_left_arrow);

static const char *TAG = "WIFI";
static char wifi_ssid[64] = {0};
static char wifi_pass[64] = {0};
static lv_obj_t *kb;
static lv_obj_t *ta_ssid;
static lv_obj_t *ta_passw;
static lv_obj_t *page_wifi;
static lv_obj_t *btn_conn;
static lv_obj_t *lb_conn;

/**
 * @brief Set the wifi creds object in Non Volatile Storage
 */
void set_wifi_creds(void)
{
    esp_err_t err;
    wifi_creds_t creds = {};
    err = app_nvs_get_wifi_creds(&creds);
    if (err != ESP_OK)
    {
        strcpy(wifi_ssid, "Enter Wifi Name");
        strcpy(wifi_pass, "Enter Wifi Pass");
    }
    else
    {
        strcpy(wifi_ssid, creds.ssid);
        strcpy(wifi_pass, creds.pass);
    }
}

/**
 * @brief Create a wifi screen object
 */
void create_wifi_screen(void)
{
    esp_err_t err;
    set_wifi_creds();
    // Background
    gui_anim_out_all(lv_scr_act(), GUI_ANIM_FAST);
    gui_anim_bg(GUI_ANIM_FAST, GUI_ACCENT_BG_1, GUI_BG_SMALL);

    // Title
    lv_obj_t *title = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(title, "Wi-Fi");
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
    page_wifi = lv_page_create(lv_scr_act(), NULL);
    lv_obj_set_pos(page_wifi, 0, TITLE_BG_OVERFLOW);
    lv_obj_set_width(page_wifi, LV_HOR_RES_MAX);
    lv_obj_set_height(page_wifi, -GUI_BG_SMALL);
    lv_page_set_scrl_layout(page_wifi, LV_LAYOUT_CENTER);
    lv_obj_set_style_local_pad_all(
        page_wifi, LV_PAGE_PART_SCROLLABLE, LV_STATE_DEFAULT, 10);
    lv_obj_set_style_local_pad_inner(
        page_wifi, LV_PAGE_PART_SCROLLABLE, LV_STATE_DEFAULT, 10);
    gui_anim_in(page_wifi, GUI_ANIM_SLOW);

    // SSID
    // Container SSID
    lv_obj_t *cont_ssid = lv_cont_create(page_wifi, NULL);
    lv_cont_set_layout(cont_ssid, LV_LAYOUT_CENTER);
    lv_obj_set_drag_parent(cont_ssid, true);
    // lv_cont_set_fit(cont_ssid, LV_FIT_NONE);
    lv_obj_set_height(cont_ssid, 40);
    lv_obj_set_width(cont_ssid, 200);
    lv_theme_apply(cont_ssid, (lv_theme_style_t)GUI_THEME_BOX_BORDER);
    // Text area SSID
    ta_ssid = lv_textarea_create(cont_ssid, NULL);
    lv_obj_set_width(ta_ssid, 180);
    lv_textarea_set_placeholder_text(ta_ssid, wifi_ssid);
    lv_textarea_set_one_line(ta_ssid, true);
    lv_textarea_set_cursor_hidden(ta_ssid, true);
    lv_theme_apply(ta_ssid, (lv_theme_style_t)LV_THEME_CONT);
    lv_textarea_set_text_align(ta_ssid, LV_LABEL_ALIGN_CENTER);
    lv_obj_set_event_cb(ta_ssid, ta_event_cb);
    // lv_textarea_set_text(ta_ssid, "Maxi"); // please hack me (^_^')

    // PASSWORD
    // Container PASSWORD
    lv_obj_t *cont_passwd = lv_cont_create(page_wifi, NULL);
    lv_cont_set_layout(cont_passwd, LV_LAYOUT_CENTER);
    lv_obj_set_drag_parent(cont_passwd, true);
    // lv_cont_set_fit(cont_passwd, LV_FIT_NONE);
    lv_obj_set_height(cont_passwd, 40);
    lv_obj_set_width(cont_passwd, 200);
    lv_theme_apply(cont_passwd, (lv_theme_style_t)GUI_THEME_BOX_BORDER);
    // Text area SSID PASSWORD
    ta_passw = lv_textarea_create(cont_passwd, NULL);
    lv_obj_set_width(ta_passw, 180);
    lv_textarea_set_placeholder_text(ta_passw, wifi_pass);
    lv_textarea_set_one_line(ta_passw, true);
    lv_textarea_set_cursor_hidden(ta_passw, true);
    lv_theme_apply(ta_passw, (lv_theme_style_t)LV_THEME_CONT);
    lv_textarea_set_text_align(ta_passw, LV_LABEL_ALIGN_CENTER);
    lv_obj_set_event_cb(ta_passw, ta_event_cb);
    // lv_textarea_set_text(ta_passw, "notmyrealpassword"); // please hack me (^_^')

    // Wifi credential
    wifi_creds_t creds = {};
    err = app_nvs_get_wifi_creds(&creds);
    if (err || !creds.valid)
    {
        lv_textarea_set_text(ta_ssid, "");
        lv_textarea_set_text(ta_passw, "");
    }
    else
    {
        lv_textarea_set_text(ta_ssid, creds.ssid);
        lv_textarea_set_text(ta_passw, creds.pass);
    }

    // Connect Button
    btn_conn = lv_btn_create(page_wifi, NULL);
    lv_obj_set_event_cb(btn_conn, btn_connect_cb);
    // lv_btn_set_fit2(btn_conn, LV_FIT_NONE, LV_FIT_TIGHT);
    lv_obj_set_width(btn_conn, 100);
    lv_obj_set_height(btn_conn, 40);
    lb_conn = lv_label_create(btn_conn, NULL);
    if (app_state_get_wifi_connected())
    {
        lv_label_set_text(lb_conn, "Disconnect");
    }
    else
    {
        lv_label_set_text(lb_conn, "Connect");
    }
    lv_obj_set_drag_parent(btn_conn, true);
    lv_btn_toggle(btn_conn);
}

/**
 * @brief Construct a new lv Button event cb declare object
 */
LV_EVENT_CB_DECLARE(btn_connect_cb)
{
    if (e == LV_EVENT_CLICKED)
    {
        if (app_state_get_wifi_connected())
        {
            app_state_disconnect_wifi();
            lv_label_set_text(lb_conn, "Connect");
        }
        else
        {
            esp_err_t err;
            wifi_creds_t creds = {};
            const char *ssid = lv_textarea_get_text(ta_ssid);
            const char *pass = lv_textarea_get_text(ta_passw);
            strcpy(creds.ssid, ssid);
            strcpy(creds.pass, pass);
            creds.valid = true;
            err = app_nvs_set_wifi_creds(&creds);
            if (err != ESP_OK)
            {
                ESP_LOGI(TAG, "could not set credentials !");
            }
            app_state_connect_wifi((char *)ssid, (char *)pass);
            create_pairing_screen();
        }
    }
}

/**
 * @brief Construct a new lv Text Area event cb declare object
 */
LV_EVENT_CB_DECLARE(ta_event_cb)
{
    if (e == LV_EVENT_RELEASED)
    {
        if (kb == NULL)
        {
            lv_coord_t h = lv_obj_get_height(page_wifi);
            lv_obj_set_height(page_wifi, LV_VER_RES / 2 - TITLE_BG_OVERFLOW);
            kb = lv_keyboard_create(lv_scr_act(), NULL);
            lv_obj_set_event_cb(kb, kb_event_cb);
        }
        lv_textarea_set_cursor_hidden(obj, false);
        lv_keyboard_set_textarea(kb, obj);
    }
    else if (e == LV_EVENT_DEFOCUSED)
    {
        lv_textarea_set_cursor_hidden(obj, true);
    }
}

/**
 * @brief Construct a new lv Keyboard event cb declare object
 */
LV_EVENT_CB_DECLARE(kb_event_cb)
{
    lv_keyboard_def_event_cb(kb, e);

    if (e == LV_EVENT_CANCEL || e == LV_EVENT_APPLY)
    {
        if (kb)
        {
            lv_coord_t h = lv_obj_get_height(page_wifi);
            lv_obj_set_height(page_wifi, -GUI_BG_SMALL);
            lv_obj_del(kb);
            kb = NULL;
        }
    }
}

/**
 * @brief Construct a new lv Icon event cb declare object
 */
LV_EVENT_CB_DECLARE(return_icon_event_cb)
{
    if (e == LV_EVENT_CLICKED)
    {
        create_start_screen();
    }
}
