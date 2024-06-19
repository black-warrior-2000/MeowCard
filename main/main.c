#include <stdio.h>
#include "esp_system.h"
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



/*variable declaration*/
i2c_obj_t i2c0_master;

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

    /*Led init*/
    led_init();
    /*i2c init*/
    i2c0_master = iic_init(I2C_NUM_0); 
    /*lcd init*/
    spi2_init();
    /*xl9555 init*/
    xl9555_init(i2c0_master);
    lcd_init();

    /*main loop*/
    while(1){
        lcd_show_string(10, 40, 240, 32, 32, "MeowCard", RED);
        vTaskDelay(500);
    }
}
