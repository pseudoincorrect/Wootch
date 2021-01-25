#ifndef __gui_animation_h__
#define __gui_animation_h__

#include "lvgl.h"

lv_anim_value_t anim_path_triangle(const lv_anim_path_t * path,
        const lv_anim_t * a);
        
void gui_anim_bg(uint32_t delay, lv_color_t color, int32_t y_new);

void gui_anim_out_all(lv_obj_t * obj, uint32_t delay);

void gui_anim_in(lv_obj_t * obj, uint32_t delay);

void anim_bg_color_cb(lv_anim_t * a, lv_anim_value_t v);


#endif