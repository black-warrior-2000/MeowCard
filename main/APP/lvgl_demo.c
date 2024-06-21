#include "lvgl_demo.h"
#include "lcd.h" //include "spi.h"
#include "esp_timer.h"
#include "lvgl.h"
#include "demos/lv_demos.h"


//extern lcd_obj_t lcd_self;  //inside "lcd.h"

/*+1 for the  pixel of rectangle Vertices  */
static void lvgl_disp_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map){

    /*draw area*/
    lcd_draw_rectangle(area->x1, area->y1, area->x2+1, area->y2+1, color_map);
    /*!! IMPORTANT : notify LVGL that the flush is ready*/
    lv_disp_flush_ready(drv);
}



/**
 * Initialize the display module
 * 
 * This function initializes the display hardware, allocates double buffers, and registers the display driver to the LVGL library.
 * It is called during the system initialization process to set up the display environment for LVGL.
 */
void lv_port_disp_init(){
    void *buf1 = NULL;
    void *buf2 = NULL;

    /* Initialize SPI2 & LCD */
    spi2_init();
    lcd_init();
    //lcd_set_rotation(LANDSCAPE);


//double buffer

//static lv_color_t buf[VER*HOR] allsize
    buf1 = heap_caps_malloc(lcd_self.width * 30 * sizeof(lv_color_t), MALLOC_CAP_DMA);
    buf2 = heap_caps_malloc(lcd_self.width * 30 * sizeof(lv_color_t), MALLOC_CAP_DMA);

    /*initialize the display buffer & notify lvgl about the buffers*/
    static lv_disp_draw_t disp_buf;
    lv_disp_draw_buf_init(&disp_buf, buf1, buf2, lcd_self.width * 30);

    /*initialize the display driver & register it to lvgl*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);

    disp_drv.hor_res = lcd_self.width;
    disp_drv.ver_res = lcd_self.height;

    disp_drv.flush_cb = lvgl_disp_flush_cb;

    disp_drv.draw_buf = &disp_buf;

    /*spi-lcd don't need user_data */
    //disp_drv.user_data = panel_handle;

    lv_disp_drv_register(&disp_drv);
    
}

void lv_port_indev_init(){

}

void lvgl_demo(void){
    /* Initialize LVGL */
    lv_init();
    lv_port_disp_init();
    lv_port_indev_init();

    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &lvgl_tick_inc,
        .name = "lvgl_tick"
    };
}

