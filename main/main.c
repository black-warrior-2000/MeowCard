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
#include "esp_lcd.h"
#include "exit.h"

/*application*/
#include "lvgl_demo.h"
//#include "cardpool.h"
#include "drawcard.h"

#define DEFAULT_FD_NUM 5
#define DEFAULT_MOUNT_POINT "/spiffs"
#define WRITE_DATA "ALIENTEK ESP32-S3\r\n"


/* 包含SPIFFS文件系统的头文件，用于在ESP32上实现基于SPI Flash的文件系统 */
#include "esp_spiffs.h"
/* 包含虚拟文件系统（VFS）的头文件，用于在ESP32上提供统一的文件系统接口 */
#include "esp_vfs.h"
/* 包含NVS（Non-Volatile Storage）的头文件，用于在ESP32上提供非易失性存储功能 */
#include "nvs_flash.h"


/*variable declaration*/
i2c_obj_t i2c0_master;
static const char *TAG = "LVGL_DEMO";
static const char *FFS_TAG = "spiffs";
extern weight_of_card_t normal_weight[5];
extern card_pool_t system_card_pool;
extern card_pool_t user_card_pool;
uint32_t test_random_array[1000]={0};

esp_err_t spiffs_init(char *partition_label,char *mount_point,size_t max_files)
{
 /* 配置 spiffs 文件系统各个参数 */
 esp_vfs_spiffs_conf_t conf = {
 .base_path = mount_point,
 .partition_label = partition_label,
 .max_files = max_files,
 .format_if_mount_failed = true,
 };
 /* 使用上面定义的设置来初始化和挂载 SPIFFS 文件系统 */
 esp_err_t ret_val = esp_vfs_spiffs_register(&conf);
 /* 判断 SPIFFS 挂载及初始化是否成功 */
 if (ret_val != ESP_OK)
 {
 if (ret_val == ESP_FAIL)
 {
 printf("Failed to mount or format filesystem\n");
 }
 else if (ret_val == ESP_ERR_NOT_FOUND)
 {
 printf("Failed to find SPIFFS partition\n");
 }
 else
 {
 printf("Failed to initialize SPIFFS(%s)\n",esp_err_to_name(ret_val));
 }
 return ESP_FAIL;
 }
 /* 打印 SPIFFS 存储信息 */
 size_t total = 0, used = 0;
 ret_val = esp_spiffs_info(conf.partition_label, &total, &used);
 if (ret_val != ESP_OK)
 {
 ESP_LOGE(FFS_TAG,
 "Failed to get SPIFFS partition information(%s)",
 esp_err_to_name(ret_val));
 }
 else
 {
 ESP_LOGE(FFS_TAG, "Partition size: total: %d, used: %d", total, used);
 }
 return ret_val;
}


/**
* @brief 注销 spiffs 初始化
* @param partition_label：分区表标识
* @retval 无
*/
esp_err_t spiffs_deinit(char *partition_label)
{
 return esp_vfs_spiffs_unregister(partition_label);
}

void spiffs_test(void)
{
 ESP_LOGI(FFS_TAG, "Opening file");
 /* 建立一个名为/spiffs/hello.txt 的只写文件 */
 FILE* f = fopen("/spiffs/hello.txt", "w");
 if (f == NULL)
 {
 ESP_LOGE(FFS_TAG, "Failed to open file for writing");
 return;
 }
 /* 写入字符 */
 fprintf(f, WRITE_DATA);
 fclose(f);
 ESP_LOGI(FFS_TAG, "File written");
 /* 重命名之前检查目标文件是否存在 */
 struct stat st;
 if (stat("/spiffs/foo.txt", &st) == 0) /* 获取文件信息，获取成功返回 0 */
 {
 /* 从文件系统中删除一个名称。
 如果名称是文件的最后一个连接，并且没有其它进程将文件打开，
 名称对应的文件会实际被删除。 */
 unlink("/spiffs/foo.txt");
 }
 /* 重命名创建的文件 */
 ESP_LOGI(FFS_TAG, "Renaming file");
 if (rename("/spiffs/hello.txt", "/spiffs/foo.txt") != 0)
 {
 ESP_LOGE(FFS_TAG, "Rename failed");
 return;
 }
 /* 打开重命名的文件并读取 */
 ESP_LOGI(FFS_TAG, "Reading file");
 f = fopen("/spiffs/foo.txt", "r");
 if (f == NULL)
 {
 ESP_LOGE(FFS_TAG, "Failed to open file for reading");
 return;
 }
 char line[64];
 fgets(line, sizeof(line), f);
 fclose(f);
 
 char* pos = strchr(line, '\n'); /* 指针 pos 指向第一个找到‘\n’ */
 if (pos)
 {
 *pos = '\0'; /* 将‘\n’替换为‘\0’ */
 }
 ESP_LOGI(FFS_TAG, "Read from file: '%s'", line);


 //lcd_show_string(90, 110, 200, 16, 16, line, RED);
}


//the main function
void app_main(void)
{
    uint32_t test_random_num=120;
    card_t temp_card;
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

    /*exit init*/
    //exit_init(); // do it in init(lvgl_demo)
    
    // /*i2c init*/
    i2c0_master = iic_init(I2C_NUM_0); 
    // /*lcd init*/
    //spi2_init();
    // /*xl9555 init*/
    xl9555_init(i2c0_master);

#ifdef CONFIG_BY_USE_ESP_LCD_PANEL

    esp_lcd_init(&io_handle, NULL, NULL, 8, 8, false);
#else
    lcd_init();
#endif
    spiffs_init("storage", DEFAULT_MOUNT_POINT, DEFAULT_FD_NUM);/*SPIFFS 初始化*/
    ESP_LOGI(TAG, "lcd_draw_rectangle testing...");
    // /*main loop*/
    // while(1){
    //     lcd_show_string(10, 40, 240, 32, 32, "MeowCard", RED);
    //     vTaskDelay(500);
    // }
    //lcd_show_string(10, 40, 240, 32, 32, "MeowCard", RED);
    //lcd_draw_rectangle(0,0,30,30,0xF800);
    vTaskDelay(1000);
   // spiffs_test();                                                  /* SPIFFS测试 */

    system_card_pool_init();
    
    system_list_out_pool_card();
    //system_list_out_pool_card_non_stop();

    /* 测试随机数生成器 */
    /*
    for(int i=0;i<100;i++){
        test_random_num=draw_card(normal_weight,1);
        ESP_LOGI(TAG, "draw_card test end,result:%u",(unsigned int)test_random_num);
        test_random_array[i] = test_random_num;
        vTaskDelay(50);
    }
    */
    /*
    for(int i=0;i<100;i++){
        ESP_LOGI(TAG, "test_random_array[%d]:%lu",i,test_random_array[i]);
        vTaskDelay(10);
    }
    */
    test_random_num=draw_card(normal_weight,1);
    find_card(&system_card_pool,test_random_num);
    /*
    temp_card = system_card_pool.card_list[test_random_num];
    for(i=0;i<7;i++) {
        temp_card.card_id
    }
    update_card(&user_card_pool,test_random_num,temp_card);
    */
    ESP_LOGI(TAG, "lcd_draw_rectangle test end...");
    ESP_LOGI(TAG, "LVGL Demo starting...");
    lvgl_demo();
}
