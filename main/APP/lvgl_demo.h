#ifndef __LVGL_DEMO_H__
#define __LVGL_DEMO_H__

#include "lvgl.h"


/*function declaration*/

/*demo*/
void lvgl_demo(void);
void lvgl_demo_part_a(void);
void lvgl_demo_part_b(void);

/*initial & register display device*/
void lvgl_port_disp_init(void);

/*initial & register input device*/
void lvgl_port_indev_init(void);

/* read touchpad */
void touchpad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data); 

/*notify lvgl about the running time*/
static void increase_lvgl_tick(void *arg);

/*set lvgl about the flushing area*/
static void lvgl_disp_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map); 

#endif