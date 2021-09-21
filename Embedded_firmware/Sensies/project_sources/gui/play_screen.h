#ifndef __play_screen_h__
#define __play_screen_h__

#include "lvgl.h"
#include "imu_types.h"


void create_play_screen(void);

imu_raw_data_t screen_imu;

typedef struct square_data_struct
{
    int x;
    int y;
    int len;
    lv_color_t color;
} square_data_t;

#endif