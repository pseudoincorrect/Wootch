#include <stdio.h>
#include <stdlib.h>
#include "lvgl.h"
#include "screens.h"

#define CANVAS_WIDTH  100
#define CANVAS_HEIGHT 100

////////////////////////////////////////////////////////////////////////////////
// Function prototypes

// IMU (Inertial Management Unit)
static void imu_update(lv_task_t * task);
static void imu_create(lv_obj_t* parent);
static void draw_square(lv_obj_t* canvas, square_data_t* square);
static void copy_square(square_data_t* square_dest,
                        square_data_t* square_to_copy);
static void redraw_square(lv_obj_t* canvas, square_data_t* square_erase,
                          square_data_t* square_draw, lv_color_t color_erase);
static void refresh_imu_canvas(imu_data_t* acc);

// Wifi
static void wifi_connect_create(lv_obj_t* parent);
static void ta_event_cb(lv_obj_t * ta, lv_event_t e);
static void kb_event_cb(lv_obj_t * _kb, lv_event_t e);
static void bt_connect_cb(lv_obj_t * bt, lv_event_t event);
static void bt_disconnect_cb(lv_obj_t * bt, lv_event_t event);

// AWS-IOT (Amazon Web Services - Internet of Things)
static void aws_iot_create(lv_obj_t* parent);

////////////////////////////////////////////////////////////////////////////////
// Static and global variables

// Top level Gui
static const char *TAG = "SCREENS";
static screen_cb_t screen_cb;
static lv_obj_t * tv;

// IMU (Inertial Management Unit)
static lv_obj_t * t2;
static lv_obj_t *cv_imu;
static square_data_t sqr_imu;
static square_data_t sqr_imu_old;
static lv_task_t * imu_update_task;

// Wi-Fi
static lv_obj_t * t1;
static lv_style_t style_box;
static lv_obj_t * kb;

// AWS-IOT (Amazon Web Services - Internet of Things)
static lv_obj_t * t3;


// #### ##    ## #### ########
//  ##  ###   ##  ##     ##
//  ##  ####  ##  ##     ##
//  ##  ## ## ##  ##     ##
//  ##  ##  ####  ##     ##
//  ##  ##   ###  ##     ##
// #### ##    ## ####    ##

////////////////////////////////////////////////////////////////////////////////
void gui_init_cb(gui_wifi_connect_cb_t wifi_connect_cb,
                 gui_wifi_disconnect_cb_t wifi_disconnect_cb)
{
    screen_cb.wifi_connect = wifi_connect_cb;
    screen_cb.wifi_disconnect = wifi_disconnect_cb;
}

////////////////////////////////////////////////////////////////////////////////
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
    lv_tabview_set_tab_act(tv, 2, LV_ANIM_ON);

    imu_data_t screen_imu = {0};
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

////////////////////////////////////////////////////////////////////////////////
static void wifi_connect_create(lv_obj_t* parent)
{
    lv_page_set_scrl_layout(parent, LV_LAYOUT_COLUMN_LEFT);

    // SSID
    // Container SSID
    lv_obj_t * cont_ssid = lv_cont_create(parent, NULL);
    lv_cont_set_layout(cont_ssid, LV_LAYOUT_ROW_MID);
    lv_obj_add_style(cont_ssid, LV_CONT_PART_MAIN, &style_box);
    lv_obj_set_drag_parent(cont_ssid, true);
    lv_cont_set_fit2(cont_ssid, LV_FIT_TIGHT, LV_FIT_TIGHT);
    // Label SSID
    lv_obj_t * label_ssid = lv_label_create(cont_ssid, NULL);
    lv_label_set_text(label_ssid, "SSID");
    // Text area SSID
    lv_obj_t * ta_ssid = lv_textarea_create(cont_ssid, NULL);
    lv_obj_set_width(ta_ssid, 200);
    lv_textarea_set_text(ta_ssid, "");
    lv_textarea_set_placeholder_text(ta_ssid, "SSID (wifi name)");
    lv_textarea_set_one_line(ta_ssid, true);
    lv_textarea_set_cursor_hidden(ta_ssid, true);
    lv_obj_set_event_cb(ta_ssid, ta_event_cb);

    // PASSW
    // Container PASSW
    lv_obj_t * cont_passw = lv_cont_create(parent, NULL);
    lv_cont_set_layout(cont_passw, LV_LAYOUT_ROW_MID);
    lv_obj_add_style(cont_passw, LV_CONT_PART_MAIN, &style_box);
    lv_obj_set_drag_parent(cont_passw, true);
    lv_cont_set_fit2(cont_passw, LV_FIT_TIGHT, LV_FIT_TIGHT);
    // Label PASSW
    lv_obj_t * label_passw = lv_label_create(cont_passw, NULL);
    lv_label_set_text(label_passw, "PASSWORD");
    // Text area PASSW
    lv_obj_t * ta_passw = lv_textarea_create(cont_passw, NULL);
    lv_obj_set_width(ta_passw, 200);
    lv_textarea_set_text(ta_passw, "");
    lv_textarea_set_placeholder_text(ta_passw, "Wifi password");
    lv_textarea_set_one_line(ta_passw, true);
    lv_textarea_set_cursor_hidden(ta_passw, true);
    lv_obj_set_event_cb(ta_passw, ta_event_cb);

    // Connect / Disconnect Buttons
    // Container Buttons
    lv_obj_t * cont_conn = lv_cont_create(parent, NULL);
    lv_cont_set_layout(cont_conn, LV_LAYOUT_ROW_MID);
    lv_obj_add_style(cont_conn, LV_CONT_PART_MAIN, &style_box);
    lv_obj_set_drag_parent(cont_conn, true);
    lv_cont_set_fit2(cont_conn, LV_FIT_TIGHT, LV_FIT_TIGHT);
    // Connect Button
    lv_obj_t * btn_conn = lv_btn_create(cont_conn, NULL);
    lv_obj_set_event_cb(btn_conn, bt_connect_cb);
    lv_btn_set_fit2(btn_conn, LV_FIT_NONE, LV_FIT_TIGHT);
    lv_obj_set_width(btn_conn, 100);
    lv_obj_t * lb_conn = lv_label_create(btn_conn, NULL);
    lv_label_set_text(lb_conn, "Connect");
    lv_btn_toggle(btn_conn);
    // Disconnect Button
    lv_obj_t * btn_discn = lv_btn_create(cont_conn, NULL);
    lv_obj_set_event_cb(btn_discn, bt_disconnect_cb);
    lv_btn_set_fit2(btn_discn, LV_FIT_NONE, LV_FIT_TIGHT);
    lv_obj_set_width(btn_discn, 100);
    lv_obj_t * lb_discn = lv_label_create(btn_discn, NULL);
    lv_label_set_text(lb_discn, "Disonnect");
    // lv_btn_set_state(btn_discn, LV_BTN_STATE_DISABLED);
    lv_btn_set_state(btn_discn, LV_BTN_STATE_RELEASED);
    lv_btn_toggle(btn_discn);
}

// #### ##     ## ##     ##
//  ##  ###   ### ##     ##
//  ##  #### #### ##     ##
//  ##  ## ### ## ##     ##
//  ##  ##     ## ##     ##
//  ##  ##     ## ##     ##
// #### ##     ##  #######

////////////////////////////////////////////////////////////////////////////////
static void imu_create(lv_obj_t* parent)
{
    lv_page_set_scrl_layout(parent, LV_LAYOUT_CENTER);

    lv_obj_t * ct_canvas;

    // CANVAS
    ct_canvas = lv_cont_create(parent, NULL);
    lv_obj_set_auto_realign(ct_canvas, true);
    lv_cont_set_layout(ct_canvas, LV_LAYOUT_CENTER);
    lv_obj_set_width(ct_canvas, 220);
    lv_obj_set_height(ct_canvas, 220);
    // lv_theme_t *th = lv_theme_mono_init(0, NULL);
    // lv_theme_set_current(th);
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
    // draw_square(cv_imu, &sqr_imu);
}

////////////////////////////////////////////////////////////////////////////////
static void imu_update(lv_task_t * task)
{
    refresh_imu_canvas(&screen_imu);
}

////////////////////////////////////////////////////////////////////////////////
static void refresh_imu_canvas(imu_data_t* acc)
{
    // sqr_imu.x = 10 + rand() % 180;
    // sqr_imu.y = 10 + rand() % 180;
    sqr_imu.x = - (screen_imu.g_x * 100 / 9000 + 100);
    sqr_imu.y = screen_imu.g_y * 100 / 9000 + 100;
    // put some limits
    sqr_imu.x = sqr_imu.x > 190 ? 190 : sqr_imu.x;
    sqr_imu.x = sqr_imu.x < -190 ? -190 : sqr_imu.x;
    sqr_imu.y = sqr_imu.y > 190 ? 190 : sqr_imu.y;
    sqr_imu.y = sqr_imu.y < -190 ? -190 : sqr_imu.y;

    sqr_imu.color = LV_COLOR_WHITE;
    redraw_square(cv_imu, &sqr_imu_old, &sqr_imu, LV_COLOR_BLACK);
}

////////////////////////////////////////////////////////////////////////////////
static void copy_square(square_data_t* square_dest,
                        square_data_t* square_to_copy)
{
    memcpy(square_dest, square_to_copy, sizeof(square_data_t));
}

////////////////////////////////////////////////////////////////////////////////
static void redraw_square(lv_obj_t* canvas, square_data_t* square_erase,
                          square_data_t* square_draw, lv_color_t color_erase)
{
    square_erase->color = color_erase;
    draw_square(canvas, square_erase);
    draw_square(canvas, square_draw);
    copy_square(square_erase, square_draw);
}

////////////////////////////////////////////////////////////////////////////////
static void draw_square(lv_obj_t* canvas, square_data_t* square)
{
    int x = square->x;
    int y = square->y;
    int len = square->len;
    lv_color_t color = square->color;

    for(int i = x; i < x + len; i++)
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

////////////////////////////////////////////////////////////////////////////////
static void aws_iot_create(lv_obj_t* parent)
{
    lv_page_set_scrl_layout(parent, LV_LAYOUT_COLUMN_LEFT);

    // Image
    // Container Image
    lv_obj_t * cont_img = lv_cont_create(parent, NULL);
    lv_cont_set_layout(cont_img, LV_LAYOUT_CENTER);
    lv_obj_add_style(cont_img, LV_CONT_PART_MAIN, &style_box);
    lv_obj_set_drag_parent(cont_img, true);
    lv_cont_set_fit2(cont_img, LV_FIT_TIGHT, LV_FIT_TIGHT);

    LV_IMG_DECLARE(img_watchdog);
    lv_obj_t * icon = lv_img_create(cont_img, NULL);
    lv_img_set_src(icon, &img_watchdog);
    lv_obj_set_width(icon, img_watchdog.header.w);
    lv_obj_set_height(icon, img_watchdog.header.h);
    lv_img_set_auto_size(icon, true);
}

//  ######     ###    ##       ##       ########     ###     ######  ##    ##
// ##    ##   ## ##   ##       ##       ##     ##   ## ##   ##    ## ##   ##
// ##        ##   ##  ##       ##       ##     ##  ##   ##  ##       ##  ##
// ##       ##     ## ##       ##       ########  ##     ## ##       #####
// ##       ######### ##       ##       ##     ## ######### ##       ##  ##
// ##    ## ##     ## ##       ##       ##     ## ##     ## ##    ## ##   ##
//  ######  ##     ## ######## ######## ########  ##     ##  ######  ##    ##

////////////////////////////////////////////////////////////////////////////////
static void bt_connect_cb(lv_obj_t * bt, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED)
    {
        printf("bt_connect_cb\n");
        screen_cb.wifi_connect("Maxi", "cornimont");
    }
}

////////////////////////////////////////////////////////////////////////////////
static void bt_disconnect_cb(lv_obj_t * bt, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED)
    {
        printf("bt_disconnect_cb\n");
        screen_cb.wifi_disconnect();
    }
}

////////////////////////////////////////////////////////////////////////////////
static void ta_event_cb(lv_obj_t * ta, lv_event_t event)
{
    if(event == LV_EVENT_RELEASED)
    {
        if(kb == NULL)
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
    else if(event == LV_EVENT_DEFOCUSED)
    {
        lv_textarea_set_cursor_hidden(ta, true);
    }
}

////////////////////////////////////////////////////////////////////////////////
static void kb_event_cb(lv_obj_t * kb, lv_event_t event)
{
    lv_keyboard_def_event_cb(kb, event);

    if(event == LV_EVENT_CANCEL)
    {
        if(kb)
        {
            lv_obj_set_height(tv, LV_VER_RES);
            lv_obj_del(kb);
            kb = NULL;
        }
    }
}
