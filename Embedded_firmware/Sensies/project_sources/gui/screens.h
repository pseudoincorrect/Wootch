#ifndef __screens_h__
#define __screens_h__

#include "lvgl.h"

typedef void (*gui_wifi_disconnect_cb_t)(void);
typedef void (*gui_wifi_connect_cb_t)(char *, char *);

typedef struct screen_cb_struct
{
    gui_wifi_connect_cb_t wifi_connect;
    gui_wifi_disconnect_cb_t wifi_disconnect;
} screen_cb_t;

typedef struct square_data_struct
{
    int x;
    int y;
    int len;
    lv_color_t color;
} square_data_t;

typedef struct accelero_data_struct
{
    int g_x;
    int g_y;
    int g_z;
    int r_x;
    int r_y;
    int r_z;
} accelero_data_t;

accelero_data_t screen_accelero;

void gui_init_cb(gui_wifi_connect_cb_t wifi_connect_cb,
                 gui_wifi_disconnect_cb_t wifi_disconnect_cb);

void start_gui(void);

#endif