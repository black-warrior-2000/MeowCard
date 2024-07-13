/* This group of includes are for the LCD panel driver framework provided by ESP-IDF */
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_commands.h"

/* This group of includes are for memory management */
#include "esp_heap_caps.h"

#include "esp_err.h"
#include "esp_log.h"

/* These includes are for GPIO and SPI communication with the LCD panel */
#include "driver/gpio.h"
#include "xl9555.h"
#include "spi.h"
#include "driver/gpio.h"


#if CONFIG_EXAMPLE_LCD_CONTROLLER_ILI9341
#include "esp_lcd_ili9341.h"
#elif CONFIG_EXAMPLE_LCD_CONTROLLER_GC9A01
#include "esp_lcd_gc9a01.h"
#elif CONFIG_EXAMPLE_LCD_CONTROLLER_ST7789
#include "esp_lcd_st7789.h"
#
#endif

/* 引脚定义 */
    
#define ESP_LCD_NUM_DC      GPIO_NUM_40
#define ESP_LCD_NUM_CS      GPIO_NUM_21
#define ESP_MOSI_GPIO_PIN   GPIO_NUM_11         /* SPI2_MOSI */ //IO_MUX 
#define ESP_CLK_GPIO_PIN    GPIO_NUM_12         /* SPI2_CLK */
#define ESP_MISO_GPIO_PIN   GPIO_NUM_13         /* SPI2_MISO */ //IO_MUX

#define ESP_LCD_SPI_HOST SPI2_HOST

#define ESP_LCD_PIXEL_CLOCK_HZ  (20 * 1000 * 1000)
#define ESP_LCD_CMD_BITS     8
#define ESP_LCD_PARAM_BITS   8


/* IO操作 */
#define ESP_LCD_PWR(x)       do{ x ? \
                            (xl9555_pin_write(SLCD_PWR_IO, 1)): \
                            (xl9555_pin_write(SLCD_PWR_IO, 0)); \
                        }while(0)

#define ESP_LCD_RST(x)       do{ x ? \
                            (xl9555_pin_write(SLCD_RST_IO, 1)): \
                            (xl9555_pin_write(SLCD_RST_IO, 0)); \
                        }while(0)

#define CONFIG_BY_USE_ESP_LCD_PANEL
#define CONFIG_BY_LCD_CONTROLLER_ST7789V
/* 
 * 全局声明LCD面板的句柄，以便在不同函数或模块间共享LCD控制权限。
 * 这个句柄是ESP-IDF框架中用于操作LCD面板的基础。
 */
extern esp_lcd_panel_handle_t panel_handle;
extern esp_lcd_panel_io_handle_t io_handle;
extern void esp_lcd_init(esp_lcd_panel_io_handle_t *io_handle,
                     esp_lcd_panel_io_color_trans_done_cb_t on_color_trans_done,
                     void *user_data, 
                     int cmd_bits, 
                     int param_bits, 
                     bool oct_mode);















