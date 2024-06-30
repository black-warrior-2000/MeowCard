#include <stdio.h>
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"

/*freertos*/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"


/*bsp driver*/
#include "lcd.h"
#include "iic.h"
#include "led.h"
#include "xl9555.h"

/*application*/
#include "lvgl_demo.h"



/*variable declaration*/
i2c_obj_t i2c0_master;
static const char *TAG = "LVGL_DEMO";

//the main function
void app_main(void)
{
    /*esp system init*/
    esp_err_t ret;
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    //esp_log_level_set("*", ESP_LOG_INFO); // 设置所有组件的日志级别为INFO
    /*Led init*/
    led_init();
    // /*i2c init*/
    i2c0_master = iic_init(I2C_NUM_0); 
    // /*lcd init*/
     spi2_init();
    // /*xl9555 init*/
    xl9555_init(i2c0_master);
     lcd_init();

    ESP_LOGI(TAG, "lcd_draw_rectangle testing...");
    // /*main loop*/
    // while(1){
    //     lcd_show_string(10, 40, 240, 32, 32, "MeowCard", RED);
    //     vTaskDelay(500);
    // }
    lcd_show_string(10, 40, 240, 32, 32, "MeowCard", RED);
    lcd_draw_rectangle(0,0,30,30,0xF800);
    vTaskDelay(1000);
    ESP_LOGI(TAG, "lcd_draw_rectangle test end...");
    ESP_LOGI(TAG, "LVGL Demo starting...");
    lvgl_demo();
}
