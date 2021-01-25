#ifndef __gui_theme_h__
#define __gui_theme_h__


/*********************
 *      INCLUDES
 *********************/
#include "lvgl.h"

/*********************
 *      DEFINES
 *********************/
/*Bg positions*/
#define GUI_BG_NONE (-LV_VER_RES)
#define GUI_BG_FULL 0
#define GUI_BG_NORMAL (-2 * (LV_VER_RES / 3))
#define GUI_BG_SMALL (-5 * (LV_VER_RES / 6))

/*Sizes*/
#define GUI_BTN_H   (50)
#define GUI_BTN_W   (200)

/*Animations*/
#define GUI_ANIM_Y (LV_VER_RES / 20)
#define GUI_ANIM_DELAY (40)
#define GUI_ANIM_TIME  (150)
#define GUI_ANIM_TIME_BG  (300)

/*Padding*/
#define GUI_TITLE_PAD 35

/*Colors*/
#define GUI_WHITE           lv_color_hex(0xffffff)
#define GUI_LIGHT           lv_color_hex(0xf3f8fe)
#define GUI_GRAY            lv_color_hex(0x8a8a8a)
#define GUI_LIGHT_GRAY      lv_color_hex(0xc4c4c4)
#define GUI_BLUE            lv_color_hex(0x2f3243) //006fb6
#define GUI_GREEN           lv_color_hex(0x4cb242)
#define GUI_RED             lv_color_hex(0xd51732)

/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
    GUI_THEME_TITLE = _LV_THEME_BUILTIN_LAST,
    GUI_THEME_LABEL_WHITE,
    GUI_THEME_ICON,
    GUI_THEME_BTN_BORDER,
    GUI_THEME_BTN_CIRCLE,
    GUI_THEME_BOX_BORDER,
    GUI_THEME_BTN_BACK
}gui_theme_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/


/**
 * Initialize the default
 * @param color_primary the primary color of the theme
 * @param color_secondary the secondary color for the theme
 * @param flags ORed flags starting with `LV_THEME_DEF_FLAG_...`
 * @param font_small pointer to a small font
 * @param font_normal pointer to a normal font
 * @param font_subtitle pointer to a large font
 * @param font_title pointer to a extra large font
 * @return a pointer to reference this theme later
 */
lv_theme_t * gui_theme_init(lv_color_t color_primary, lv_color_t color_secondary, uint32_t flags,
                                    const lv_font_t * font_small, const lv_font_t * font_normal, const lv_font_t * font_subtitle,
                                    const lv_font_t * font_title);


#endif