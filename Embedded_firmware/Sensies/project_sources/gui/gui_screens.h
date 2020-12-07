#ifndef __gui_screens_h__
#define __gui_screens_h__

#include "lvgl.h"
#include "imu_types.h"

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

imu_raw_data_t screen_imu;

typedef enum watch_state_enum
{
    ON_WATCH,
    OFF_WATCH
} watch_state_t;

void gui_init_cb(gui_wifi_connect_cb_t wifi_connect_cb,
                 gui_wifi_disconnect_cb_t wifi_disconnect_cb);

void start_gui(void);

#endif