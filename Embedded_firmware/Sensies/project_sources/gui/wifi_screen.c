#include "wifi_screen.h"
#include "gui_animation.h"
#include "gui_theme.h"
#include "watch_screen.h"
#include "start_screen.h"
#include "esp_log.h"


/**********************
 *  STATIC PROTOTYPES
 **********************/
LV_EVENT_CB_DECLARE(btn_connect_cb);
LV_EVENT_CB_DECLARE(ta_event_cb);
LV_EVENT_CB_DECLARE(kb_event_cb);


/**********************
 *  GLOBAL VARIABLES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
// Wi-Fi
static const char *TAG = "WIFI";
static char wifi_ssid[32] = {0};
static char wifi_pass[32] = {0};
// static lv_obj_t * win;
static lv_obj_t * kb;
static lv_obj_t * ta_ssid;
static lv_obj_t * ta_passw;
static lv_obj_t * cont_wifi;

/*******************************************************************************
 * @brief
 * @param
 */
void create_wifi_screen(void)
{
    // Background
    uint32_t title_bg_pos = GUI_BG_SMALL;
    gui_anim_out_all(lv_scr_act(), GUI_ANIM_FAST);
    gui_anim_bg(GUI_ANIM_FAST, GUI_ACCENT_BG_1, title_bg_pos);

    // Title
    lv_obj_t * title = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(title, "Connect to your Wi-Fi");
    lv_obj_set_style_local_text_color(
        title, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, GUI_ACCENT_FG_1);
    lv_obj_set_style_local_text_font(
        title, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_theme_get_font_title());
    lv_obj_align(title, NULL, LV_ALIGN_IN_TOP_MID, 0, 13);
    gui_anim_in(title, GUI_ANIM_FAST);

    // Main container
    cont_wifi = lv_cont_create(lv_scr_act(), NULL);
    lv_obj_set_style_local_bg_color(
        cont_wifi, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, GUI_BG_0);
    lv_obj_set_width(cont_wifi, LV_HOR_RES_MAX);
    lv_obj_set_height(cont_wifi, -title_bg_pos);
    lv_cont_set_layout(cont_wifi, LV_LAYOUT_CENTER);
    lv_obj_set_style_local_pad_all(
        cont_wifi, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 25);
    lv_obj_set_style_local_pad_inner(
        cont_wifi, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 10);
    lv_obj_align(cont_wifi, lv_scr_act(), LV_ALIGN_CENTER, 0, 10);
    lv_obj_set_pos(cont_wifi, 0, LV_VER_RES + title_bg_pos);

    // SSID
    // Container SSID
    lv_obj_t * cont_ssid = lv_cont_create(cont_wifi, NULL);
    lv_cont_set_layout(cont_ssid, LV_LAYOUT_CENTER);
    lv_obj_set_drag_parent(cont_ssid, true);
    // lv_cont_set_fit(cont_ssid, LV_FIT_NONE);
    lv_obj_set_height(cont_ssid, 40);
    lv_obj_set_width(cont_ssid, 200);
    lv_theme_apply(cont_ssid, (lv_theme_style_t)GUI_THEME_BOX_BORDER);
    // Text area SSID
    ta_ssid = lv_textarea_create(cont_ssid, NULL);
    lv_obj_set_width(ta_ssid, 180);
    lv_textarea_set_text(ta_ssid, "");
    lv_textarea_set_placeholder_text(ta_ssid, "WiFi SSID (name)");
    lv_textarea_set_one_line(ta_ssid, true);
    lv_textarea_set_cursor_hidden(ta_ssid, true);
    lv_theme_apply(ta_ssid, (lv_theme_style_t)LV_THEME_CONT);
    lv_textarea_set_text_align(ta_ssid, LV_LABEL_ALIGN_CENTER);
    lv_obj_set_event_cb(ta_ssid, ta_event_cb);
    lv_textarea_set_text(ta_ssid, "Maxi"); // please hack me (^_^')

    // PASSWORD
    // Container PASSWORD
    lv_obj_t * cont_passwd = lv_cont_create(cont_wifi, NULL);
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
    lv_textarea_set_placeholder_text(ta_passw, "WiFi Password");
    lv_textarea_set_one_line(ta_passw, true);
    lv_textarea_set_cursor_hidden(ta_passw, true);
    lv_theme_apply(ta_passw, (lv_theme_style_t)LV_THEME_CONT);
    lv_textarea_set_text_align(ta_passw, LV_LABEL_ALIGN_CENTER);
    // lv_obj_set_event_cb(ta_passw, ta_event_cb);
    lv_textarea_set_text(ta_passw, "notmyrealpassword"); // please hack me (^_^')

    // Connect Button
    lv_obj_t * btn_conn = lv_btn_create(cont_wifi, NULL);
    lv_obj_set_event_cb(btn_conn, btn_connect_cb);
    lv_btn_set_fit2(btn_conn, LV_FIT_NONE, LV_FIT_TIGHT);
    lv_obj_set_width(btn_conn, 100);
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
        const char* ssid = lv_textarea_get_text(ta_ssid);
        const char* pass = lv_textarea_get_text(ta_passw);
        // screen_cb.wifi_connect((char*) ssid, (char*) pass);
        ESP_LOGI(TAG, "wifi connect %s , pass %s\n", ssid, pass);

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
        ESP_LOGI(TAG, "ta_event_cb LV_EVENT_RELEASED\n");
        if(kb == NULL)
        {
            lv_obj_set_height(cont_wifi, LV_VER_RES / 2);
            kb = lv_keyboard_create(lv_scr_act(), NULL);
            lv_obj_set_event_cb(kb, kb_event_cb);
        }
        lv_textarea_set_cursor_hidden(obj, false);
        lv_keyboard_set_textarea(kb, obj);
    }
    else if(e == LV_EVENT_DEFOCUSED)
    {
        ESP_LOGI(TAG, "ta_event_cb LV_EVENT_DEFOCUSED\n");
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

    if(e == LV_EVENT_CANCEL)
    {
        if(kb)
        {
            lv_obj_set_height(cont_wifi, LV_VER_RES);
            lv_obj_del(kb);
            kb = NULL;
        }
    }
}
