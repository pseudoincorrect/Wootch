#include "old_gui.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lvgl.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "app_state.h"
// #include "esp_log.h"

////////////////////////////////////////////////////////////////////////////////
// DEFINES
#define CANVAS_WIDTH 100
#define CANVAS_HEIGHT 100

////////////////////////////////////////////////////////////////////////////////
// FUNCTION PROTOTYPES
// IMU (Inertial Management Unit)
static void imu_update(lv_task_t *task);
static void imu_create(lv_obj_t *parent);
static void draw_square(lv_obj_t *canvas, square_data_t *square);
static void copy_square(square_data_t *square_dest,
                        square_data_t *square_to_copy);
static void redraw_square(lv_obj_t *canvas, square_data_t *square_erase,
                          square_data_t *square_draw, lv_color_t color_erase);
static void refresh_imu_canvas(imu_raw_data_t *acc);
// Wifi
static void wifi_connect_create(lv_obj_t *parent);
static void ta_event_cb(lv_obj_t *ta, lv_event_t e);
static void kb_event_cb(lv_obj_t *_kb, lv_event_t e);
static void btn_connect_cb(lv_obj_t *bt, lv_event_t event);
static void btn_disconnect_cb(lv_obj_t *bt, lv_event_t event);
// AWS-IOT (Amazon Web Services - Internet of Things)
static void aws_iot_create(lv_obj_t *parent);
static void toggle_img(watch_state_t state);
static void btn_toggle_img_cb(lv_obj_t *bt, lv_event_t event);
static void btn_alert_cb(lv_obj_t *btn, lv_event_t evt);

////////////////////////////////////////////////////////////////////////////////
// GLOBAL AND STATIC VARIABLES
// Top level Gui
static const char *TAG = "SCREENS";
static screen_cb_t screen_cb;
static lv_obj_t *tv;
// IMU (Inertial Management Unit)
static lv_obj_t *t2;
static lv_obj_t *cv_imu;
static square_data_t sqr_imu;
static square_data_t sqr_imu_old;
static lv_task_t *imu_update_task;
// Wi-Fi
char wifi_ssid[32] = {0};
char wifi_pass[32] = {0};
static lv_obj_t *t1;
static lv_style_t style_box;
static lv_obj_t *kb;
static lv_obj_t *ta_ssid;
static lv_obj_t *ta_passw;
// AWS-IOT (Amazon Web Services - Internet of Things)
static lv_obj_t *t3;
static lv_obj_t *cont_dog;
static lv_obj_t *img_dog;
static bool on_watch;
static lv_obj_t *mbox, hey;

// #### ##    ## #### ########
//  ##  ###   ##  ##     ##
//  ##  ####  ##  ##     ##
//  ##  ## ## ##  ##     ##
//  ##  ##  ####  ##     ##
//  ##  ##   ###  ##     ##
// #### ##    ## ####    ##

/*******************************************************************************
 * @brief
 * @param
 */
void gui_init_cb(gui_wifi_connect_cb_t wifi_connect_cb,
                 gui_wifi_disconnect_cb_t wifi_disconnect_cb)
{
    screen_cb.wifi_connect = wifi_connect_cb;
    screen_cb.wifi_disconnect = wifi_disconnect_cb;
}

/*******************************************************************************
 * @brief
 * @param
 */
void start_gui(void)
{
    tv = lv_tabview_create(lv_scr_act(), NULL);
    t1 = lv_tabview_add_tab(tv, "IMU");
    t2 = lv_tabview_add_tab(tv, "WIFI");
    t3 = lv_tabview_add_tab(tv, "AWS-IOT");

    LV_THEME_DEFAULT_INIT(lv_theme_get_color_primary(),
                          lv_theme_get_color_secondary(),
                          LV_THEME_MATERIAL_FLAG_DARK,
                          lv_theme_get_font_small(), lv_theme_get_font_normal(),
                          lv_theme_get_font_subtitle(), lv_theme_get_font_title());

    lv_style_init(&style_box);
    lv_style_set_value_align(&style_box, LV_STATE_DEFAULT, LV_ALIGN_OUT_TOP_LEFT);
    lv_style_set_margin_top(&style_box, LV_STATE_DEFAULT, LV_DPX(5));

    imu_create(t1);
    wifi_connect_create(t2);
    aws_iot_create(t3);
    lv_tabview_set_tab_act(tv, 1, LV_ANIM_ON);

    imu_raw_data_t screen_imu = {0};
    imu_update_task = lv_task_create(imu_update, 100,
                                     LV_TASK_PRIO_MID, NULL);
}

// ##      ## #### ######## ####
// ##  ##  ##  ##  ##        ##
// ##  ##  ##  ##  ##        ##
// ##  ##  ##  ##  ######    ##
// ##  ##  ##  ##  ##        ##
// ##  ##  ##  ##  ##        ##
//  ###  ###  #### ##       ####

/*******************************************************************************
 * @brief
 * @param
 */
static void wifi_connect_create(lv_obj_t *parent)
{
    lv_page_set_scrl_layout(parent, LV_LAYOUT_COLUMN_LEFT);

    // SSID
    // Container SSID
    lv_obj_t *cont_ssid = lv_cont_create(parent, NULL);
    lv_cont_set_layout(cont_ssid, LV_LAYOUT_ROW_MID);
    lv_obj_add_style(cont_ssid, LV_CONT_PART_MAIN, &style_box);
    lv_obj_set_drag_parent(cont_ssid, true);
    lv_cont_set_fit2(cont_ssid, LV_FIT_TIGHT, LV_FIT_TIGHT);
    // Label SSID
    lv_obj_t *label_ssid = lv_label_create(cont_ssid, NULL);
    lv_label_set_text(label_ssid, "SSID");
    // Text area SSID
    ta_ssid = lv_textarea_create(cont_ssid, NULL);
    lv_obj_set_width(ta_ssid, 200);
    lv_textarea_set_text(ta_ssid, "");
    lv_textarea_set_placeholder_text(ta_ssid, "SSID (wifi name)");
    lv_textarea_set_one_line(ta_ssid, true);
    lv_textarea_set_cursor_hidden(ta_ssid, true);
    lv_obj_set_event_cb(ta_ssid, ta_event_cb);
    lv_textarea_set_text(ta_ssid, "Maxi"); // please hack me (^_^')

    // PASSW
    // Container PASSW
    lv_obj_t *cont_passw = lv_cont_create(parent, NULL);
    lv_cont_set_layout(cont_passw, LV_LAYOUT_ROW_MID);
    lv_obj_add_style(cont_passw, LV_CONT_PART_MAIN, &style_box);
    lv_obj_set_drag_parent(cont_passw, true);
    lv_cont_set_fit2(cont_passw, LV_FIT_TIGHT, LV_FIT_TIGHT);
    // Label PASSW
    lv_obj_t *label_passw = lv_label_create(cont_passw, NULL);
    lv_label_set_text(label_passw, "PASSWORD");
    // Text area PASSW
    ta_passw = lv_textarea_create(cont_passw, NULL);
    lv_obj_set_width(ta_passw, 200);
    lv_textarea_set_text(ta_passw, "");
    lv_textarea_set_placeholder_text(ta_passw, "Wifi password");
    lv_textarea_set_one_line(ta_passw, true);
    lv_textarea_set_cursor_hidden(ta_passw, true);
    lv_obj_set_event_cb(ta_passw, ta_event_cb);
    lv_textarea_set_text(ta_passw, "notmyrealpassword"); // please hack me (^_^')

    // Connect / Disconnect Buttons
    // Container Buttons
    lv_obj_t *cont_conn = lv_cont_create(parent, NULL);
    lv_cont_set_layout(cont_conn, LV_LAYOUT_ROW_MID);
    lv_obj_add_style(cont_conn, LV_CONT_PART_MAIN, &style_box);
    lv_obj_set_drag_parent(cont_conn, true);
    lv_cont_set_fit2(cont_conn, LV_FIT_TIGHT, LV_FIT_TIGHT);
    // Connect Button
    lv_obj_t *btn_conn = lv_btn_create(cont_conn, NULL);
    lv_obj_set_event_cb(btn_conn, btn_connect_cb);
    lv_btn_set_fit2(btn_conn, LV_FIT_NONE, LV_FIT_TIGHT);
    lv_obj_set_width(btn_conn, 100);
    lv_obj_t *lb_conn = lv_label_create(btn_conn, NULL);
    lv_label_set_text(lb_conn, "Connect");
    lv_btn_toggle(btn_conn);
    // Disconnect Button
    lv_obj_t *btn_discn = lv_btn_create(cont_conn, NULL);
    lv_obj_set_event_cb(btn_discn, btn_disconnect_cb);
    lv_btn_set_fit2(btn_discn, LV_FIT_NONE, LV_FIT_TIGHT);
    lv_obj_set_width(btn_discn, 100);
    lv_obj_t *lb_discn = lv_label_create(btn_discn, NULL);
    lv_label_set_text(lb_discn, "Disconnect");
    // lv_btn_set_state(btn_discn, LV_BTN_STATE_DISABLED);
    lv_btn_set_state(btn_discn, LV_BTN_STATE_RELEASED);
    lv_btn_toggle(btn_discn);
}

/*******************************************************************************
 * @brief
 * @param
 */
static void btn_connect_cb(lv_obj_t *bt, lv_event_t event)
{
    if (event == LV_EVENT_CLICKED)
    {
        const char *ssid = lv_textarea_get_text(ta_ssid);
        const char *pass = lv_textarea_get_text(ta_passw);
        screen_cb.wifi_connect((char *)ssid, (char *)pass);
    }
}

/*******************************************************************************
 * @brief
 * @param
 */
static void btn_disconnect_cb(lv_obj_t *bt, lv_event_t event)
{
    if (event == LV_EVENT_CLICKED)
    {
        screen_cb.wifi_disconnect();
    }
}

/*******************************************************************************
 * @brief
 * @param
 */
static void ta_event_cb(lv_obj_t *ta, lv_event_t event)
{
    if (event == LV_EVENT_RELEASED)
    {
        if (kb == NULL)
        {
            lv_obj_set_height(tv, LV_VER_RES / 2);
            kb = lv_keyboard_create(lv_scr_act(), NULL);
            lv_obj_set_event_cb(kb, kb_event_cb);

            lv_indev_wait_release(lv_indev_get_act());
        }
        lv_textarea_set_cursor_hidden(ta, false);
        lv_page_focus(t1, lv_textarea_get_label(ta), LV_ANIM_ON);
        lv_keyboard_set_textarea(kb, ta);
    }
    else if (event == LV_EVENT_DEFOCUSED)
    {
        lv_textarea_set_cursor_hidden(ta, true);
    }
}

/*******************************************************************************
 * @brief
 * @param
 */
static void kb_event_cb(lv_obj_t *_, lv_event_t event)
{
    lv_keyboard_def_event_cb(kb, event);

    if (event == LV_EVENT_CANCEL)
    {
        if (kb)
        {
            lv_obj_set_height(tv, LV_VER_RES);
            lv_obj_del(kb);
            kb = NULL;
        }
    }
}

// #### ##     ## ##     ##
//  ##  ###   ### ##     ##
//  ##  #### #### ##     ##
//  ##  ## ### ## ##     ##
//  ##  ##     ## ##     ##
//  ##  ##     ## ##     ##
// #### ##     ##  #######

/*******************************************************************************
 * @brief
 * @param
 */
static void imu_create(lv_obj_t *parent)
{
    lv_page_set_scrl_layout(parent, LV_LAYOUT_CENTER);

    lv_obj_t *ct_canvas;

    // CANVAS
    ct_canvas = lv_cont_create(parent, NULL);
    lv_obj_set_auto_realign(ct_canvas, true);
    lv_cont_set_layout(ct_canvas, LV_LAYOUT_CENTER);
    lv_obj_set_width(ct_canvas, 220);
    lv_obj_set_height(ct_canvas, 220);
    static lv_color_t cbuf[LV_CANVAS_BUF_SIZE_INDEXED_1BIT(200, 200)];
    cv_imu = lv_canvas_create(ct_canvas, NULL);
    lv_canvas_set_buffer(cv_imu, cbuf, 200, 200, LV_IMG_CF_INDEXED_1BIT);
    lv_obj_align(cv_imu, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_canvas_set_palette(cv_imu, 0, LV_COLOR_BLACK);
    lv_canvas_set_palette(cv_imu, 1, LV_COLOR_WHITE);

    sqr_imu.x = 10;
    sqr_imu.y = 10;
    sqr_imu.len = 10;
    sqr_imu.color = LV_COLOR_WHITE;
}

/*******************************************************************************
 * @brief
 * @param
 */
static void imu_update(lv_task_t *task)
{
    refresh_imu_canvas(&screen_imu);
}

/*******************************************************************************
 * @brief
 * @param
 */
static void refresh_imu_canvas(imu_raw_data_t *acc)
{
    imu_raw_data_t imu_raw_data = {0};

    imu_raw_data_t *last_imu_raw_data = app_state_get_last_imu_raw_data();

    memcpy(&imu_raw_data, &last_imu_raw_data, sizeof(imu_raw_data_t));

    sqr_imu.x = -(imu_raw_data.g_x * 100 / 9000 + 100);
    sqr_imu.y = imu_raw_data.g_y * 100 / 9000 + 100;

    // put some limits
    sqr_imu.x = sqr_imu.x > 190 ? 190 : sqr_imu.x;
    sqr_imu.x = sqr_imu.x < -190 ? -190 : sqr_imu.x;
    sqr_imu.y = sqr_imu.y > 190 ? 190 : sqr_imu.y;
    sqr_imu.y = sqr_imu.y < -190 ? -190 : sqr_imu.y;

    sqr_imu.color = LV_COLOR_WHITE;
    redraw_square(cv_imu, &sqr_imu_old, &sqr_imu, LV_COLOR_BLACK);
}

/*******************************************************************************
 * @brief
 * @param
 */
static void copy_square(square_data_t *square_dest,
                        square_data_t *square_to_copy)
{
    memcpy(square_dest, square_to_copy, sizeof(square_data_t));
}

/*******************************************************************************
 * @brief
 * @param
 */
static void redraw_square(lv_obj_t *canvas, square_data_t *square_erase,
                          square_data_t *square_draw, lv_color_t color_erase)
{
    square_erase->color = color_erase;
    draw_square(canvas, square_erase);
    draw_square(canvas, square_draw);
    copy_square(square_erase, square_draw);
}

/*******************************************************************************
 * @brief
 * @param
 */
static void draw_square(lv_obj_t *canvas, square_data_t *square)
{
    int x = square->x;
    int y = square->y;
    int len = square->len;
    lv_color_t color = square->color;

    for (int i = x; i < x + len; i++)
        for (int j = y; j < y + len; j++)
            lv_canvas_set_px(canvas, i, j, color);
}

//    ###    ##      ##  ######
//   ## ##   ##  ##  ## ##    ##
//  ##   ##  ##  ##  ## ##
// ##     ## ##  ##  ##  ######
// ######### ##  ##  ##       ##
// ##     ## ##  ##  ## ##    ##
// ##     ##  ###  ###   ######

/*******************************************************************************
 * @brief
 * @param
 */
static void aws_iot_create(lv_obj_t *parent)
{
    lv_page_set_scrl_layout(parent, LV_LAYOUT_ROW_MID);

    // Image watch dog
    // Container image watch dog
    cont_dog = lv_cont_create(parent, NULL);
    lv_cont_set_layout(cont_dog, LV_LAYOUT_CENTER);
    lv_obj_add_style(cont_dog, LV_CONT_PART_MAIN, &style_box);
    lv_obj_set_drag_parent(cont_dog, true);
    lv_cont_set_fit2(cont_dog, LV_FIT_TIGHT, LV_FIT_TIGHT);
    // Image watch dog
    on_watch = OFF_WATCH;
    toggle_img(on_watch);

    // Button toggle
    lv_obj_t *btn_toggle_img = lv_btn_create(parent, NULL);
    lv_obj_set_event_cb(btn_toggle_img, btn_toggle_img_cb);
    lv_btn_set_fit2(btn_toggle_img, LV_FIT_NONE, LV_FIT_TIGHT);
    lv_obj_set_width(btn_toggle_img, 100);
    lv_obj_t *lb_toggle_img = lv_label_create(btn_toggle_img, NULL);
    lv_label_set_text(lb_toggle_img, "toggle img");
    lv_btn_toggle(btn_toggle_img);

    // Button toggle
    lv_obj_t *btn_alert = lv_btn_create(parent, NULL);
    lv_obj_set_event_cb(btn_alert, btn_alert_cb);
    lv_btn_set_fit2(btn_alert, LV_FIT_NONE, LV_FIT_TIGHT);
    lv_obj_set_width(btn_alert, 100);
    lv_obj_t *lb_alert = lv_label_create(btn_alert, NULL);
    lv_label_set_text(lb_alert, "Alert !");
    lv_btn_toggle(btn_alert);
}

static void toggle_img(watch_state_t state)
{
    if (img_dog != NULL)
        lv_obj_del(img_dog);

    if (state == ON_WATCH)
    {
        LV_IMG_DECLARE(img_watch_dog);
        img_dog = lv_img_create(cont_dog, NULL);
        lv_img_set_src(img_dog, &img_watch_dog);
        lv_obj_set_width(img_dog, img_watch_dog.header.w);
        lv_obj_set_height(img_dog, img_watch_dog.header.h);
        lv_img_set_auto_size(img_dog, true);
    }
    else
    {
        LV_IMG_DECLARE(img_sleepy_dog);
        img_dog = lv_img_create(cont_dog, NULL);
        lv_img_set_src(img_dog, &img_sleepy_dog);
        lv_obj_set_width(img_dog, img_sleepy_dog.header.w);
        lv_obj_set_height(img_dog, img_sleepy_dog.header.h);
        lv_img_set_auto_size(img_dog, true);
    }
}

/*******************************************************************************
 * @brief
 * @param
 */
static void btn_toggle_img_cb(lv_obj_t *bt, lv_event_t event)
{
    if (event == LV_EVENT_CLICKED)
    {
        toggle_img(on_watch);
        on_watch = on_watch == ON_WATCH ? OFF_WATCH : ON_WATCH;
    }
}

static void opa_anim(void *bg, lv_anim_value_t v)
{
    lv_obj_set_style_local_bg_opa(bg, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, v);
}

static void mbox_event_cb(lv_obj_t *obj, lv_event_t evt)
{
    if (evt == LV_EVENT_DELETE && obj == mbox)
    {
        /* Delete the parent modal background */
        lv_obj_del_async(lv_obj_get_parent(mbox));
        mbox = NULL; /* happens before object is actually deleted! */
    }
    else if (evt == LV_EVENT_VALUE_CHANGED)
    {
        /* A button was clicked */
        lv_msgbox_start_auto_close(mbox, 0);
    }
}

static void btn_alert_cb(lv_obj_t *btn, lv_event_t evt)
{
    if (evt == LV_EVENT_CLICKED)
    {
        display_alert();
    }
}

void display_alert(void)
{
    if (mbox == NULL)
    {
        /* Create a base object for the modal background */
        lv_obj_t *obj = lv_obj_create(lv_scr_act(), NULL);
        lv_obj_reset_style_list(obj, LV_OBJ_PART_MAIN);
        lv_obj_add_style(obj, LV_OBJ_PART_MAIN, &style_box);
        lv_obj_set_pos(obj, 0, 0);
        lv_obj_set_size(obj, LV_HOR_RES, LV_VER_RES);

        static const char *btns2[] = {"Ok", "Cancel", ""};

        /* Create the message box as a child of the modal background */
        mbox = lv_msgbox_create(obj, NULL);
        lv_msgbox_add_btns(mbox, btns2);
        lv_msgbox_set_text(mbox, "Hello world!");
        lv_obj_align(mbox, NULL, LV_ALIGN_CENTER, 0, 0);
        lv_obj_set_event_cb(mbox, mbox_event_cb);
    }
}