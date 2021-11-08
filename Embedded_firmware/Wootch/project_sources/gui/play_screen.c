#include "play_screen.h"
#include "start_screen.h"
#include "gui_animation.h"
#include "gui_theme.h"
#include "esp_log.h"
#include "esp_err.h"
#include "app_state.h"

#define CANVAS_WIDTH 100
#define CANVAS_HEIGHT 100
#define TITLE_BG_OVERFLOW (LV_VER_RES + GUI_BG_SMALL)

static void imu_update(lv_task_t *task);
static void imu_create(lv_obj_t *parent);
static void draw_square(lv_obj_t *canvas, square_data_t *square);
static void copy_square(square_data_t *square_dest,
                        square_data_t *square_to_copy);
static void redraw_square(lv_obj_t *canvas, square_data_t *square_erase,
                          square_data_t *square_draw, lv_color_t color_erase);
static void refresh_imu_canvas(imu_raw_data_t *acc);
LV_EVENT_CB_DECLARE(return_icon_event_cb);

static lv_obj_t *page_imu;
static lv_obj_t *t2;
static lv_obj_t *cv_imu;
static square_data_t sqr_imu;
static square_data_t sqr_imu_old;
static lv_task_t *imu_update_task;

LV_IMG_DECLARE(icon_left_arrow);

/**
 * @brief Create a play screen object
 */
void create_play_screen(void)
{
    // Background
    gui_anim_out_all(lv_scr_act(), GUI_ANIM_FAST);
    gui_anim_bg(GUI_ANIM_FAST, GUI_ACCENT_BG_1, GUI_BG_SMALL);

    // Title
    lv_obj_t *title = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(title, "IMU Play");
    lv_obj_set_style_local_text_color(
        title, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, GUI_ACCENT_FG_1);
    lv_obj_set_style_local_text_font(
        title, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_theme_get_font_title());
    lv_obj_align(title, NULL, LV_ALIGN_IN_TOP_MID, 0, 13);
    gui_anim_in(title, GUI_ANIM_SLOW);

    // Return Button/Icon
    lv_obj_t *btn_ret = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_event_cb(btn_ret, return_icon_event_cb);
    // lv_btn_set_fit2(btn_ret, LV_FIT_NONE, LV_FIT_TIGHT);
    lv_obj_set_width(btn_ret, 40);
    lv_obj_set_height(btn_ret, 40);
    lv_obj_set_click(btn_ret, true);
    gui_anim_in(btn_ret, GUI_ANIM_SLOW);
    lv_obj_align(btn_ret, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    lv_obj_set_style_local_bg_color(
        btn_ret, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, GUI_ACCENT_BG_1);
    lv_obj_t *img_left = lv_img_create(btn_ret, NULL);
    lv_img_set_src(img_left, &icon_left_arrow);
    lv_img_set_antialias(img_left, false);
    lv_obj_set_width(img_left, icon_left_arrow.header.w);
    lv_obj_set_height(img_left, icon_left_arrow.header.h);
    lv_obj_align(img_left, NULL, LV_ALIGN_CENTER, 0, 0);

    // Main container
    page_imu = lv_page_create(lv_scr_act(), NULL);
    lv_obj_set_pos(page_imu, 0, TITLE_BG_OVERFLOW);
    lv_obj_set_width(page_imu, LV_HOR_RES_MAX);
    lv_obj_set_height(page_imu, -GUI_BG_SMALL);
    lv_page_set_scrl_layout(page_imu, LV_LAYOUT_CENTER);
    lv_obj_set_style_local_pad_all(
        page_imu, LV_PAGE_PART_SCROLLABLE, LV_STATE_DEFAULT, 10);
    lv_obj_set_style_local_pad_inner(
        page_imu, LV_PAGE_PART_SCROLLABLE, LV_STATE_DEFAULT, 10);
    gui_anim_in(page_imu, GUI_ANIM_SLOW);

    imu_create(page_imu);

    imu_raw_data_t screen_imu = {0};
    imu_update_task = lv_task_create(imu_update, 100,
                                     LV_TASK_PRIO_MID, NULL);
}

/**
 * @brief Create an IMU canva
 * 
 * @param parent parent object
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

/**
 * @brief update the IMU canva
 * 
 * @param task unused but needed by LVGL
 */
static void imu_update(lv_task_t *task)
{
    refresh_imu_canvas(&screen_imu);
}

/**
 * @brief Update the IMU canva with fresh IMU data
 * 
 * @param acc IMU raw data
 */
static void refresh_imu_canvas(imu_raw_data_t *acc)
{
    imu_raw_data_t imu_raw_data = {0};

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

/**
 * @brief Copy a square data
 * 
 * @param square_dest destination square data
 * @param square_to_copy data to copy into destination
 */
static void copy_square(square_data_t *square_dest,
                        square_data_t *square_to_copy)
{
    memcpy(square_dest, square_to_copy, sizeof(square_data_t));
}

/**
 * @brief Erase and draw a square on a canva
 * 
 * @param canvas objec
 * @param square_erase square object 
 * @param square_draw  square object
 * @param color_erase  color of the erasing square
 */
static void redraw_square(lv_obj_t *canvas, square_data_t *square_erase,
                          square_data_t *square_draw, lv_color_t color_erase)
{
    square_erase->color = color_erase;
    draw_square(canvas, square_erase);
    draw_square(canvas, square_draw);
    copy_square(square_erase, square_draw);
}

/**
 * @brief Draw a square on a canva
 * 
 * @param canvas object
 * @param square object
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

/**
 * @brief Construct a new lv Icon event cb declare object
 */
LV_EVENT_CB_DECLARE(return_icon_event_cb)
{
    if (e == LV_EVENT_CLICKED)
    {
        lv_task_del(imu_update_task);
        create_start_screen();
    }
}
