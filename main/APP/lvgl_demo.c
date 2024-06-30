#include "lvgl_demo.h"
#include "lcd.h" //include "spi.h"
#include "xl9555.h"
#include "esp_timer.h"
#include "lvgl.h"
#include "demos/lv_demos.h"
#include "esp_log.h"

static const char *TEST_TAG = "LVGL_DEMO";
static const char *KEY_TAG = "last_key";
//extern lcd_obj_t lcd_self;  //inside "lcd.h"
static void increase_lvgl_tick(void *arg)
{
    /* 告诉LVGL已经过了多少毫秒 */
    //ESP_LOGI(TEST_TAG, "LVGL Demo ticking...");
    lv_tick_inc(1);
}

uint32_t last_key(void){
    //ESP_LOGI(KEY_TAG, "enter...");
    static uint32_t last_key = 0;
    uint32_t new_key = xl9555_key_scan(1);
    //ESP_LOGI(KEY_TAG, "key_pressed... new_key:%ld",new_key);
    if (new_key){
    ESP_LOGI(KEY_TAG, "new key not equal 0");
        switch (new_key){
            case KEY0_PRES:
                last_key = LV_KEY_LEFT;
                ESP_LOGI(KEY_TAG, "key_pressed... LV_KEY_LEFT");
                break;
            case KEY1_PRES:
                last_key = LV_KEY_ENTER;
                ESP_LOGI(KEY_TAG, "key_pressed... LV_KEY_ENTER");
                break;
            case KEY2_PRES:
                last_key = LV_KEY_RIGHT;
                ESP_LOGI(KEY_TAG, "key_pressed... LV_KEY_RIGHT");
                break;
            default:
                break;
        }
    }
    return last_key;
}

uint32_t key_pressed(void){
    return xl9555_key_scan(1);
}

void encoder_with_keys_read(lv_indev_t * indev, lv_indev_data_t*data){

  data->key = last_key();            /*Get the last pressed or released key*/
                                     /* use LV_KEY_ENTER for encoder press */
  if(key_pressed()) {
    data->state = LV_INDEV_STATE_PRESSED;
  }
  else {
      data->state = LV_INDEV_STATE_RELEASED;
      /* Optionally you can also use enc_diff, if you have encoder*/
      //data->enc_diff = enc_get_new_moves();
  }
}

/*+1 for the  pixel of rectangle Vertices  */
static void lvgl_disp_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map){
    //ESP_LOGI(TEST_TAG, "lvgl_disp_flush_cb...%d %d %d %d %d",area->x1, area->y1, area->x2+1, area->y2+1, color_map->full);
    /*draw area*/
    
    //lcd_draw_rectangle(area->x1, area->y1, area->x2+1, area->y2+1, color_map->full);
    int32_t x, y;
    uint16_t * buf16 = (uint16_t *)color_map;
    for(y = area->y1; y <= area->y2; y++) {
        for(x = area->x1; x <= area->x2; x++) {
            lcd_draw_pixel(x, y, *buf16);
            buf16++;
        }
    }
    /*!! IMPORTANT : notify LVGL that the flush is ready*/
    lv_disp_flush_ready(drv);
}

// void my_flush_cb(lv_disp_drv_t * display, const lv_area_t * area, void * px_map)
// {
//     /*The most simple case (but also the slowest) to put all pixels to the screen one-by-one
//      *`put_px` is just an example, it needs to be implemented by you.*/
//     uint16_t * buf16 = (uint16_t *)px_map; /*Let's say it's a 16 bit (RGB565) display*/
//     int32_t x, y;
//     for(y = area->y1; y <= area->y2; y++) {
//         for(x = area->x1; x <= area->x2; x++) {
//             put_px(x, y, *buf16);
//             buf16++;
//         }
//     }

//     /* IMPORTANT!!!
//      * Inform LVGL that you are ready with the flushing and buf is not used anymore*/
//     lv_display_flush_ready(disp);
// }

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
    //spi2_init();
    //lcd_init();
    //lcd_set_rotation(LANDSCAPE);


//double buffer

//static lv_color_t buf[VER*HOR] allsize
    buf1 = heap_caps_malloc(160 * 30 * sizeof(lv_color_t), MALLOC_CAP_DMA);
    buf2 = heap_caps_malloc(160 * 30 * sizeof(lv_color_t), MALLOC_CAP_DMA);

    /*initialize the display buffer & notify lvgl about the buffers*/
    static lv_disp_draw_buf_t disp_buf;
    lv_disp_draw_buf_init(&disp_buf, buf1, buf2, 160 * 30);

    /*initialize the display driver & register it to lvgl*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    ESP_LOGI(TEST_TAG, "lv_port_disp_init...%d %d ",lcd_self.width,lcd_self.height);
    disp_drv.hor_res = 160;
    disp_drv.ver_res = 120;

    disp_drv.flush_cb = lvgl_disp_flush_cb;

    disp_drv.draw_buf = &disp_buf;

    /*spi-lcd don't need user_data */
    //disp_drv.user_data = panel_handle;

    lv_disp_drv_register(&disp_drv);
    
}

void lv_port_indev_init(){


    /*
    lv_indev_t * indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_...);   
    lv_indev_set_read_cb(indev, read_cb);  
    */

    //key_init();
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);

    indev_drv.type = LV_INDEV_TYPE_ENCODER;

    indev_drv.read_cb = encoder_with_keys_read;


    lv_indev_t *indev_encoder;
    indev_encoder = lv_indev_drv_register(&indev_drv);
}

void lvgl_demo(void){
    /* Initialize LVGL */
    lv_init();
    lv_port_disp_init();
    lv_port_indev_init();

    const esp_timer_create_args_t lvgl_tick_timer_args = {
        .callback = &increase_lvgl_tick,
        .name = "lvgl_tick"
    };

    esp_timer_handle_t lvgl_tick_timer = NULL;
    ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, 1 * 1000));

    /* 官方demo,需要在SDK Configuration中开启对应Demo */
    //lv_demo_music();      
    //lv_demo_benchmark();
    //lv_demo_widgets();
    // lv_demo_stress();
     lv_demo_keypad_encoder();

    while (1)
    {
        lv_timer_handler();             /* LVGL计时器 */
        vTaskDelay(pdMS_TO_TICKS(10));  /* 延时10毫秒 */
    }

}

