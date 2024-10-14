#include "lvgl_demo.h"
#include "lcd.h" //include "spi.h"
#include "exit.h"
#include "xl9555.h"
#include "esp_timer.h"
#include "lvgl.h"
#include "demos/lv_demos.h"
#include "examples/lv_examples.h"
#include "esp_log.h"
#include "esp_lcd.h"
#include "gui_guider.h"
#include "events_init.h"

#define CONFIG_ZD_DISP_INIT

#define CONFIG_USE_EXIT_IO

static const char *TEST_TAG = "LVGL_DEMO";
static const char *KEY_TAG = "last_key";

extern bool key_btn_flag;
bool key_btn_last_flag = 0;

lv_ui guider_ui;

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

bool btn_key_pressed(void){
    
    if (key_btn_flag){
        ESP_LOGI(KEY_TAG,"btn_key_pressed... : %d",key_btn_flag);
        key_btn_flag = 0;
        return 1;
    } 
    else{
        return 0;
    }
    
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

#ifdef CONFIG_USE_EXIT_IO


void button_with_keys_read(lv_indev_t * indev, lv_indev_data_t*data) {
    
    data->btn_id = 1;
    if(btn_key_pressed()) {
        ESP_LOGI(KEY_TAG,"btn_key_pressed... state:PRESSED");
        data->state = LV_INDEV_STATE_PRESSED;
    }
    else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

#endif



/**
 * @brief 此为LVGL显示刷新的回调函数示例
 * 
 * 该函数作为LVGL显示驱动中的回调函数，用于指示LCD面板绘制特定区域。
 * 当显示需要更新时（如UI操作或动画后），LVGL会调用此函数。
 * 
 * @param drv 指向显示驱动结构的指针，用于访问驱动特定的配置和数据。
 * @param area 指向区域结构的指针，指定需要更新的显示区域。
 * @param color_map 指向颜色数组的指针，包含需要更新区域的颜色数据。
 */
static void esp_lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map)
{
    // 从显示驱动的用户数据字段中获取LCD面板句柄
    esp_lcd_panel_handle_t panel_handle = (esp_lcd_panel_handle_t) drv->user_data;
    
    // 计算更新区域的坐标
    int offsetx1 = area->x1;
    int offsetx2 = area->x2;
    int offsety1 = area->y1;
    int offsety2 = area->y2;
    
    // 调用LCD面板绘图函数，传入面板句柄及计算后的坐标和颜色数据，
    // 以更新LCD面板的指定区域。
    // 注意，右边界和底边界的坐标加1是为了确保整个区域被更新。

    esp_lcd_panel_draw_bitmap(panel_handle, offsetx1, offsety1, offsetx2 + 1, offsety2 + 1, color_map);
    /*!! IMPORTANT : notify LVGL that the flush is ready*/
    lv_disp_flush_ready(drv);
}
/*+1 for the  pixel of rectangle Vertices  */
static void lvgl_disp_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map){
    ESP_LOGI(TEST_TAG, "lvgl_disp_flush_cb...%d %d %d %d %d",area->x1, area->y1, area->x2, area->y2, color_map->full);
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

#ifdef CONFIG_ZD_DISP_INIT
    void *buf1 = NULL;
    void *buf2 = NULL;

    /* Initialize SPI2 & LCD */
    //spi2_init();
    //lcd_init();
    //lcd_set_rotation(LANDSCAPE);


//double buffer

//static lv_color_t buf[VER*HOR] allsize
    // buf1 = heap_caps_malloc(lcd_self.width * 10 * sizeof(lv_color_t), MALLOC_CAP_DMA);
    // buf2 = heap_caps_malloc(lcd_self.width * 10 * sizeof(lv_color_t), MALLOC_CAP_DMA);


    buf1 = heap_caps_malloc(240 * 10 * sizeof(lv_color_t), MALLOC_CAP_DMA);
    buf2 = heap_caps_malloc(240 * 10 * sizeof(lv_color_t), MALLOC_CAP_DMA);
    /*initialize the display buffer & notify lvgl about the buffers*/
    static lv_disp_draw_buf_t disp_buf;
    //lv_disp_draw_buf_init(&disp_buf, buf1, buf2, lcd_self.width * 10);
    lv_disp_draw_buf_init(&disp_buf, buf1, buf2, 240 * 10);
    /*initialize the display driver & register it to lvgl*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    


#ifdef CONFIG_BY_USE_ESP_LCD_PANEL
ESP_LOGI(TEST_TAG, "lv_port_disp_init start...");
    disp_drv.hor_res = 240;
    disp_drv.ver_res = 240;
    disp_drv.flush_cb = esp_lvgl_flush_cb;
#else   
ESP_LOGI(TEST_TAG, "lv_port_disp_init start...%d %d ",lcd_self.width,lcd_self.height);
    disp_drv.hor_res = lcd_self.width;
    disp_drv.ver_res = lcd_self.height;
    disp_drv.flush_cb = lvgl_disp_flush_cb;
#endif
    disp_drv.draw_buf = &disp_buf;

#ifdef CONFIG_BY_LCD_CONTROLLER_ST7789V
    disp_drv.user_data = panel_handle;
#endif

    lv_disp_drv_register(&disp_drv);

#else
void lv_port_disp_init(void)
{
    /*-------------------------
     * Initialize your display
     * -----------------------*/
    //disp_init();

    /*-----------------------------
     * Create a buffer for drawing
     *----------------------------*/

    /**
     * LVGL requires a buffer where it internally draws the widgets.
     * Later this buffer will passed to your display driver's `flush_cb` to copy its content to your display.
     * The buffer has to be greater than 1 display row
     *
     * There are 3 buffering configurations:
     * 1. Create ONE buffer:
     *      LVGL will draw the display's content here and writes it to your display
     *
     * 2. Create TWO buffer:
     *      LVGL will draw the display's content to a buffer and writes it your display.
     *      You should use DMA to write the buffer's content to the display.
     *      It will enable LVGL to draw the next part of the screen to the other buffer while
     *      the data is being sent form the first buffer. It makes rendering and flushing parallel.
     *
     * 3. Double buffering
     *      Set 2 screens sized buffers and set disp_drv.full_refresh = 1.
     *      This way LVGL will always provide the whole rendered screen in `flush_cb`
     *      and you only need to change the frame buffer's address.
     */


    /* Example for 2) */
    static lv_disp_draw_buf_t draw_buf_dsc_2;
    static lv_color_t buf_2_1[320 * 10];                        /*A buffer for 10 rows*/
    static lv_color_t buf_2_2[320 * 10];                        /*An other buffer for 10 rows*/
    lv_disp_draw_buf_init(&draw_buf_dsc_2, buf_2_1, buf_2_2, 320 * 10);   /*Initialize the display buffer*/


    /*-----------------------------------
     * Register the display in LVGL
     *----------------------------------*/

    static lv_disp_drv_t disp_drv;                         /*Descriptor of a display driver*/
    lv_disp_drv_init(&disp_drv);                    /*Basic initialization*/

    /*Set up the functions to access to your display*/

    /*Set the resolution of the display*/
    disp_drv.hor_res = 320;
    disp_drv.ver_res = 240;

    /*Used to copy the buffer's content to the display*/
    disp_drv.flush_cb = lvgl_disp_flush_cb;

    /*Set a display buffer*/
    disp_drv.draw_buf = &draw_buf_dsc_2;


    /*Finally register the driver*/
    lv_disp_drv_register(&disp_drv);
    ESP_LOGI(TEST_TAG, "lv_port_disp_init end...%d %d ",lcd_self.width,lcd_self.height);
#endif
    
}

void lv_port_indev_init(){


    /*
    lv_indev_t * indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_...);   
    lv_indev_set_read_cb(indev, read_cb);  
    */
#ifdef CONFIG_USE_EXIT_IO

    exit_init();
#endif
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);

#ifdef CONFIG_USE_EXIT_IO

    indev_drv.type = LV_INDEV_TYPE_BUTTON;

    indev_drv.read_cb = button_with_keys_read;
    
    lv_indev_t *indev_button;
    indev_button = lv_indev_drv_register(&indev_drv);
#else
    //indev_drv.type = LV_INDEV_TYPE_POINTER;
    //indev_drv.read_cb = touchpad_read;
    //lv_indev_t *indev_encoder;
    //indev_encoder = lv_indev_drv_register(&indev_drv);
#endif




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
    //lv_demo_keypad_encoder();
    //lv_example_btn_1();

    setup_ui(&guider_ui);
    events_init(&guider_ui);
    
    while (1)
    {
        lv_timer_handler();             /* LVGL计时器 */
        vTaskDelay(pdMS_TO_TICKS(10));  /* 延时10毫秒 */
    }

}


void lvgl_demo_part_a(void){
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
    //lv_demo_keypad_encoder();
    //lv_example_btn_1();

    //setup_ui(&lvgl_by_ui);
    //events_init(&lvgl_by_ui);
    

}

void lvgl_demo_part_b(void){

    while (1)
    {
        lv_timer_handler();             /* LVGL计时器 */
        vTaskDelay(pdMS_TO_TICKS(10));  /* 延时10毫秒 */
    }

}
