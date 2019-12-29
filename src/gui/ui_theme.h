/**
 *
 */

#ifndef UI_THEME_H
#define UI_THEME_H

#ifdef __cplusplus
extern "C" {
#endif

	/*********************
	 *      INCLUDES
	 *********************/
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lv_conf.h"
#include <lvgl.h>
#else
#include "../lv_conf.h"
#endif


	 /*********************
	  *      DEFINES
	  *********************/

	  /**********************
	   *      TYPEDEFS
	   **********************/

	   /**********************
		* GLOBAL PROTOTYPES
		**********************/

	/**
	 * Initialize the ui theme
	 * @param hue [0..360] hue value from HSV color space to define the theme's base color
	 * @param font pointer to a font (NULL to use the default)
	 * @return pointer to the initialized theme
	 */
	lv_theme_t* ui_theme_init(uint16_t hue, lv_font_t* font);

	/**
	 * Get a pointer to the theme
	 * @return pointer to the theme
	 */
	lv_theme_t* ui_theme_get(void);

	/**********************
	 *      MACROS
	 **********************/



#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*UI_THEME_H*/
