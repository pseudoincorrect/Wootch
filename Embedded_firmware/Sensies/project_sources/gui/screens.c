#include <stdio.h>
#include <stdlib.h>
#include "lvgl.h"
#include "screens.h"

#define CANVAS_WIDTH  100
#define CANVAS_HEIGHT 100

////////////////////////////////////////////////////////////////////////////////
static void wifi_connect_create(lv_obj_t* parent);
static void create_accelero(lv_obj_t* parent);
static void draw_square(lv_obj_t* canvas, square_data_t* square);
static void copy_square(square_data_t* square_dest,
                        square_data_t* square_to_copy);
static void redraw_square(lv_obj_t* canvas, square_data_t* square_erase,
                          square_data_t* square_draw, lv_color_t color_erase);
static void refresh_accel_canvas(accelero_data_t* acc);
static void ta_event_cb(lv_obj_t * ta, lv_event_t e);
static void kb_event_cb(lv_obj_t * _kb, lv_event_t e);

void update_accelero(lv_task_t * task);

////////////////////////////////////////////////////////////////////////////////
static const char *TAG = "SCREENS";

static lv_obj_t * tv;
static lv_obj_t * t1;
static lv_obj_t * t2;
static lv_style_t style_box;
static lv_obj_t * kb;
static lv_obj_t *cv_accel;
static square_data_t sqr_accel, sqr_accel_old;
lv_task_t * update_accelero_task;

void update_accelero(lv_task_t * task)
{
    refresh_accel_canvas(&screen_accelero);
}

static void refresh_accel_canvas(accelero_data_t* acc)
{
    // sqr_accel.x = 10 + rand() % 180;
    // sqr_accel.y = 10 + rand() % 180;
    sqr_accel.x = - (screen_accelero.g_x * 100 / 9000 + 100);
    sqr_accel.y = screen_accelero.g_y * 100 / 9000 + 100;
    sqr_accel.color = LV_COLOR_WHITE;
    redraw_square(cv_accel, &sqr_accel_old, &sqr_accel, LV_COLOR_BLACK);
}

////////////////////////////////////////////////////////////////////////////////
void sensie_gui(void)
{
    tv = lv_tabview_create(lv_scr_act(), NULL);
    t1 = lv_tabview_add_tab(tv, "IMU");
    t2 = lv_tabview_add_tab(tv, "WiFi");

    LV_THEME_DEFAULT_INIT(lv_theme_get_color_primary(),
                          lv_theme_get_color_secondary(),
                          LV_THEME_MATERIAL_FLAG_DARK,
                          lv_theme_get_font_small(), lv_theme_get_font_normal(),
                          lv_theme_get_font_subtitle(), lv_theme_get_font_title());

    lv_style_init(&style_box);
    lv_style_set_value_align(&style_box, LV_STATE_DEFAULT, LV_ALIGN_OUT_TOP_LEFT);
    // lv_style_set_value_ofs_y(&style_box, LV_STATE_DEFAULT, - LV_DPX(10));
    lv_style_set_margin_top(&style_box, LV_STATE_DEFAULT, LV_DPX(5));

    create_accelero(t1);
    wifi_connect_create(t2);

    accelero_data_t screen_accelero = {0};
    update_accelero_task = lv_task_create(update_accelero, 100,
                                          LV_TASK_PRIO_MID, NULL);
}

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
    lv_btn_set_fit2(btn_conn, LV_FIT_NONE, LV_FIT_TIGHT);
    lv_obj_set_width(btn_conn, 100);
    lv_obj_t * lb_conn = lv_label_create(btn_conn, NULL);
    lv_label_set_text(lb_conn, "Connect");
    lv_btn_toggle(btn_conn);
    // Disconnect Button
    lv_obj_t * btn_discn = lv_btn_create(cont_conn, NULL);
    lv_btn_set_fit2(btn_discn, LV_FIT_NONE, LV_FIT_TIGHT);
    lv_obj_set_width(btn_discn, 100);
    lv_obj_t * lb_discn = lv_label_create(btn_discn, NULL);
    lv_label_set_text(lb_discn, "Disonnect");
    lv_btn_set_state(btn_discn, LV_BTN_STATE_DISABLED);
    lv_btn_toggle(btn_discn);
}
////////////////////////////////////////////////////////////////////////////////
static void create_accelero(lv_obj_t* parent)
{
    lv_page_set_scrl_layout(parent, LV_LAYOUT_ROW_MID);


    lv_obj_t * ct_input;
    lv_obj_t * ct_canvas;

    // ACCELERO CONTROLS
    ct_input = lv_cont_create(parent, NULL);
    lv_obj_set_auto_realign(ct_input, true);
    lv_cont_set_layout(ct_input, LV_LAYOUT_PRETTY_MID);
    lv_obj_set_width(ct_input, 220);
    lv_obj_set_height(ct_input, 220);
    int grid_width = lv_obj_get_width_grid(ct_input, 3, 1);
    int grid_height = lv_obj_get_height_grid(ct_input, 3, 1);
    printf("Grid width %d height %d \n", grid_width, grid_height);

    // Add a button
    lv_obj_t *obj;
    obj = lv_obj_create(ct_input, NULL);
    lv_obj_set_size(obj, grid_width, grid_height);
    lv_obj_set_style_local_border_width(obj, 0, LV_STATE_DEFAULT, 0);

    obj = lv_btn_create(ct_input, NULL);
    lv_obj_set_size(obj, grid_width, grid_height);
    obj = lv_label_create(obj, NULL);
    lv_label_set_text(obj, "UP");

    obj = lv_obj_create(ct_input, NULL);
    lv_obj_set_size(obj, grid_width, grid_height);
    lv_obj_set_style_local_border_width(obj, 0, LV_STATE_DEFAULT, 0);

    // Add a button
    obj = lv_btn_create(ct_input, NULL);
    lv_obj_set_size(obj, grid_width, grid_height);
    obj = lv_label_create(obj, NULL);
    lv_label_set_text(obj, "LEFT");
    // Add a button
    obj = lv_btn_create(ct_input, NULL);
    lv_obj_set_size(obj, grid_width, grid_height);
    obj = lv_label_create(obj, NULL);
    lv_label_set_text(obj, "DOWN");
    // Add a button
    obj = lv_btn_create(ct_input, NULL);
    lv_obj_set_size(obj, grid_width, grid_height);
    obj = lv_label_create(obj, NULL);
    lv_label_set_text(obj, "RIGHT");
    // Add a button
    obj = lv_btn_create(ct_input, NULL);
    lv_obj_set_size(obj, grid_width * 3, grid_height);
    lv_btn_toggle(obj);
    obj = lv_label_create(obj, NULL);
    lv_label_set_text(obj, "RANDOM");

    // CANVAS
    ct_canvas = lv_cont_create(parent, NULL);
    lv_obj_set_auto_realign(ct_canvas, true);
    lv_cont_set_layout(ct_canvas, LV_LAYOUT_CENTER);
    lv_obj_set_width(ct_canvas, 220);
    lv_obj_set_height(ct_canvas, 220);
    // lv_theme_t *th = lv_theme_mono_init(0, NULL);
    // lv_theme_set_current(th);
    static lv_color_t cbuf[LV_CANVAS_BUF_SIZE_INDEXED_1BIT(200, 200)];
    cv_accel = lv_canvas_create(ct_canvas, NULL);
    lv_canvas_set_buffer(cv_accel, cbuf, 200, 200, LV_IMG_CF_INDEXED_1BIT);
    lv_obj_align(cv_accel, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_canvas_set_palette(cv_accel, 0, LV_COLOR_BLACK);
    lv_canvas_set_palette(cv_accel, 1, LV_COLOR_WHITE);

    sqr_accel.x = 10;
    sqr_accel.y = 10;
    sqr_accel.len = 10;
    sqr_accel.color = LV_COLOR_WHITE;
    // draw_square(cv_accel, &sqr_accel);
}

static void copy_square(square_data_t* square_dest,
                        square_data_t* square_to_copy)
{
    memcpy(square_dest, square_to_copy, sizeof(square_data_t));
}

static void redraw_square(lv_obj_t* canvas, square_data_t* square_erase,
                          square_data_t* square_draw, lv_color_t color_erase)
{
    square_erase->color = color_erase;
    draw_square(canvas, square_erase);
    draw_square(canvas, square_draw);
    copy_square(square_erase, square_draw);
}

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

////////////////////////////////////////////////////////////////////////////////
static void ta_event_cb(lv_obj_t * ta, lv_event_t e)
{
    if(e == LV_EVENT_RELEASED)
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
    else if(e == LV_EVENT_DEFOCUSED)
    {
        lv_textarea_set_cursor_hidden(ta, true);
    }
}

////////////////////////////////////////////////////////////////////////////////
static void kb_event_cb(lv_obj_t * kb, lv_event_t e)
{
    lv_keyboard_def_event_cb(kb, e);

    if(e == LV_EVENT_CANCEL)
    {
        if(kb)
        {
            lv_obj_set_height(tv, LV_VER_RES);
            lv_obj_del(kb);
            kb = NULL;
        }
    }
}
