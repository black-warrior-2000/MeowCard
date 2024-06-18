#include <stdio.h>
#include "esp_system.h"
#include "nvs_flash.h"

/*freertos*/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"


/*bsp driver*/
#include "lcd.h"


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

    /*lcd init*/
    spi2_init();
    lcd_init();
    while(1){
        lcd_show_string(10, 40, 240, 32, 32, "MeowCard", RED);
        vTaskDelay(500);
    }
}
