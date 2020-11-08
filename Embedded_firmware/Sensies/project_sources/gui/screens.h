#ifndef __screens_h__
#define __screens_h__

#include "lvgl.h"

void sensie_gui(void);


typedef struct square_data_struct {
    int x;
    int y;
    int len;
    lv_color_t color;
} square_data_t;

typedef struct accelero_data_struct {
    int g_x;
    int g_y;
    int g_z;
    int r_x;
    int r_y;
    int r_z;
} accelero_data_t;

accelero_data_t screen_accelero;

#endif