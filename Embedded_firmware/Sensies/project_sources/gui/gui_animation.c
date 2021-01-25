#include "gui_animation.h"
#include "gui_theme.h"

extern lv_style_t style_box;
extern lv_obj_t * scan_img;
extern lv_obj_t * bg_top;
extern lv_obj_t * bg_bottom;
extern lv_color_t bg_color_prev;
extern lv_color_t bg_color_act;

/*******************************************************************************
 * Calculate the current value of an animation applying linear characteristic
 * @param a pointer to an animation
 * @return the current value to set
 */
lv_anim_value_t anim_path_triangle(const lv_anim_path_t * path,
                                   const lv_anim_t * a)
{
    /*Calculate the current step*/
    uint32_t step;
    lv_anim_value_t ret = 0;
    if(a->time == a->act_time)
    {
        ret = (lv_anim_value_t)a->end;
    }
    else
    {
        if(a->act_time < a->time / 2)
        {
            step = ((int32_t)a->act_time * 1024) / (a->time / 2);
            int32_t new_value;
            new_value = (int32_t)step * (GUI_BG_SMALL - a->start);
            new_value = new_value >> 10;
            new_value += a->start;

            ret = (lv_anim_value_t)new_value;
        }
        else
        {
            uint32_t t = a->act_time - a->time / 2;
            step = ((int32_t)t * 1024) / (a->time / 2);
            int32_t new_value;
            new_value = (int32_t)step * (a->end - GUI_BG_SMALL);
            new_value = new_value >> 10;
            new_value += GUI_BG_SMALL;

            ret = (lv_anim_value_t)new_value;
        }
    }

    return ret;
}


/*******************************************************************************
 * @brief
 * @param
 */
void gui_anim_bg(uint32_t delay, lv_color_t color, int32_t y_new)
{
    lv_coord_t y_act = lv_obj_get_y(bg_top);
    lv_color_t act_color = lv_obj_get_style_bg_color(bg_top, LV_OBJ_PART_MAIN);
    if(y_new != GUI_BG_NORMAL && y_new == y_act
            && act_color.full == color.full) return;


    lv_anim_t a;
    if((y_new == GUI_BG_NORMAL && y_new == y_act) ||
            (y_new == GUI_BG_NORMAL && y_act == GUI_BG_FULL))
    {
        lv_anim_path_t path;
        lv_anim_path_init(&path);
        lv_anim_path_set_cb(&path, anim_path_triangle);

        lv_anim_init(&a);
        lv_anim_set_var(&a, bg_top);
        lv_anim_set_time(&a, GUI_ANIM_TIME_BG + 200);
        lv_anim_set_delay(&a, delay);
        lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t) lv_obj_set_y);
        lv_anim_set_values(&a, y_act, y_new);
        lv_anim_set_path(&a, &path);
        lv_anim_start(&a);
    }
    else
    {
        lv_anim_init(&a);
        lv_anim_set_var(&a, bg_top);
        lv_anim_set_time(&a, GUI_ANIM_TIME_BG);
        lv_anim_set_delay(&a, delay);
        lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t) lv_obj_set_y);
        lv_anim_set_values(&a, lv_obj_get_y(bg_top), y_new);
        lv_anim_start(&a);
    }

    bg_color_prev = bg_color_act;
    bg_color_act = color;

    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t) anim_bg_color_cb);
    lv_anim_set_values(&a, 0, 255);
    lv_anim_set_time(&a, GUI_ANIM_TIME_BG);
    lv_anim_set_path(&a, &lv_anim_path_def);
    lv_anim_start(&a);

}

/*******************************************************************************
 * @brief
 * @param
 */
void gui_anim_out_all(lv_obj_t * obj, uint32_t delay)
{
    lv_obj_t * child = lv_obj_get_child_back(obj, NULL);
    while(child)
    {
        if(child != scan_img && child != bg_top && child != bg_bottom
                && child != lv_scr_act())
        {
            lv_anim_t a;
            lv_anim_init(&a);
            lv_anim_set_var(&a, child);
            lv_anim_set_time(&a, GUI_ANIM_TIME);
            lv_anim_set_delay(&a, delay);
            lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t) lv_obj_set_y);
            if(lv_obj_get_y(child) < 80)
            {
                lv_anim_set_values(&a, lv_obj_get_y(child),
                                   lv_obj_get_y(child) - GUI_ANIM_Y);
            }
            else
            {
                lv_anim_set_values(&a, lv_obj_get_y(child),
                                   lv_obj_get_y(child) + GUI_ANIM_Y);

                delay += GUI_ANIM_DELAY;
            }
            lv_anim_set_ready_cb(&a, lv_obj_del_anim_ready_cb);
            lv_anim_start(&a);
        }
        child = lv_obj_get_child_back(obj, child);
    }
}

/*******************************************************************************
 * @brief
 * @param
 */
void gui_anim_in(lv_obj_t * obj, uint32_t delay)
{
    if (obj != bg_top && obj != bg_bottom && obj != lv_scr_act())
    {
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, obj);
        lv_anim_set_time(&a, GUI_ANIM_TIME);
        lv_anim_set_delay(&a, delay);
        lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t) lv_obj_set_y);
        lv_anim_set_values(&a, lv_obj_get_y(obj) - GUI_ANIM_Y,
                           lv_obj_get_y(obj));
        lv_anim_start(&a);

        lv_obj_fade_in(obj, GUI_ANIM_TIME - 50, delay);
    }
}

/*******************************************************************************
 * @brief
 * @param
 */
void anim_bg_color_cb(lv_anim_t * a, lv_anim_value_t v)
{
    lv_color_t c = lv_color_mix(bg_color_act, bg_color_prev, v);
    lv_obj_set_style_local_bg_color(bg_top, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, c);
}
