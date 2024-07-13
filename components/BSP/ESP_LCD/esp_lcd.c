#ifndef ESP_LCD_H
#include "esp_lcd.h"
#endif

static const char *TAG = "lcd_panel.st7789";

esp_lcd_panel_handle_t panel_handle = NULL;
esp_lcd_panel_io_handle_t io_handle = NULL;

void esp_lcd_init(esp_lcd_panel_io_handle_t *by_io_handle,
                     esp_lcd_panel_io_color_trans_done_cb_t on_color_trans_done,
                     void *user_data, 
                     int cmd_bits, 
                     int param_bits, 
                     bool oct_mode){
    /* 配置GPIO引脚为输出模式，用于控制背光灯 */
        /*使用xl9555控制背光灯引脚，注意检查是否初始化*/
    ESP_LCD_PWR(0);

    /* 初始化SPI总线配置 */
    ESP_LOGI(TAG, "Initialize SPI bus");
    spi_bus_config_t buscfg = {
        .sclk_io_num = ESP_CLK_GPIO_PIN,  /* SPI时钟信号线引脚号 */
        .mosi_io_num = ESP_MOSI_GPIO_PIN,  /* SPI主机输出从机输入信号线引脚号 */
        .miso_io_num = -1,                    /* 不使用SPI主机输入从机输出信号线引脚号 */
        .quadwp_io_num = -1,                  /* 不使用四线制WP信号线 */
        .quadhd_io_num = -1,                  /* 不使用四线制HD信号线 */
        .max_transfer_sz = 240 * 80 * sizeof(uint16_t),  /* 设置最大传输尺寸 */
    };
    /* 初始化SPI总线，使用自动选择的DMA通道 */
    ESP_ERROR_CHECK(spi_bus_initialize(ESP_LCD_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO));

    

    // Initialize the panel IO, setting up the SPI configuration
    ESP_LOGI(TAG, "Install panel IO");
    esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = ESP_LCD_NUM_DC,  // Data command selection pin
        .cs_gpio_num = ESP_LCD_NUM_CS,  // Chip select pin
        .pclk_hz = ESP_LCD_PIXEL_CLOCK_HZ,  // Pixel clock frequency
        .lcd_cmd_bits = ESP_LCD_CMD_BITS,  // Command bit length
        .lcd_param_bits = ESP_LCD_PARAM_BITS,  // Parameter bit length
        .spi_mode = 0,  // SPI mode
        .trans_queue_depth = 10,  // Transaction queue depth
        .on_color_trans_done = on_color_trans_done,  // Callback after color transformation is complete
        .user_ctx = user_data,  // User context
    };
    // Create a new SPI panel IO instance
    // Attach the LCD to the SPI bus
    ESP_LOGI(TAG, "esp_lcd_new_panel_io_spi");
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)ESP_LCD_SPI_HOST, &io_config, by_io_handle));

    
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = -1,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_BGR,
        .bits_per_pixel = 16,
    };

#if CONFIG_EXAMPLE_LCD_CONTROLLER_ILI9341
    ESP_LOGI(TAG, "Install ILI9341 panel driver");
    ESP_ERROR_CHECK(esp_lcd_new_panel_ili9341(io_handle, &panel_config, &panel_handle));
#elif CONFIG_EXAMPLE_LCD_CONTROLLER_GC9A01
    ESP_LOGI(TAG, "Install GC9A01 panel driver");
    ESP_ERROR_CHECK(esp_lcd_new_panel_gc9a01(io_handle, &panel_config, &panel_handle));
#elif CONFIG_EXAMPLE_LCD_CONTROLLER_ST7789V
    ESP_LOGI(TAG, "Install ST7789V panel driver");

#else
    ESP_LOGI(TAG, "Install ST7789V panel driver");
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));


#endif
    /* LCD硬件复位 */
    /* 复位显示屏 */
    ESP_LOGI(TAG, "ESP_LCD_RST");
    ESP_LCD_RST(0);
    vTaskDelay(100);
    ESP_LCD_RST(1);
    vTaskDelay(100);     
    ESP_LOGI(TAG, "esp_lcd_panel_reset");   
     
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_LOGI(TAG, "esp_lcd_panel_init");  
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
#if CONFIG_EXAMPLE_LCD_CONTROLLER_GC9A01
    ESP_ERROR_CHECK(esp_lcd_panel_invert_color(panel_handle, true));
#endif
    //ESP_ERROR_CHECK(esp_lcd_panel_mirror(panel_handle, true, false));
    ESP_LOGI(TAG, "esp_lcd_panel_disp_on_off"); 
    // user can flush pre-defined pattern to the screen before we turn on the screen or backlight
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));

    ESP_ERROR_CHECK(esp_lcd_panel_invert_color(panel_handle, true));
    // turn on backlight
    ESP_LCD_PWR(1);
    ESP_LOGI(TAG, "ESP_LCD_PWR(1)");



}