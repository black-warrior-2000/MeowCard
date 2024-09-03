#include "cardpool.h"
#include "esp_log.h"

card_pool_t system_card_pool={};
card_pool_t user_card_pool={};

//#define CARD_PRECONFIG_FILE_NAME "cardpool.config"

static const char * CARDPOOL = "cardpool";
void system_card_pool_init(void)
{
    /*读取flash卡池数据*/
    system_card_pool.card_pool_name="system_card_pool";
    /*如果没有的话，初始化卡池数据*/
    for(int i=0;i<MAX_CARDS;i++){
        system_card_pool.card_pool[i].is_user_card=false;
        system_card_pool.card_pool[i].is_fetched_card=false;
        system_card_pool.card_pool[i].card_id=i;
    }
    /*如果有config 文件就读取config 文件的卡片数据*/
    

#ifdef CONFIG_CARDPOOL_PRECONFIG
    ESP_LOGI(CARDPOOL,"cardpool preconfig");

#else
    ESP_LOGI(CARDPOOL,"cardpool no preconfig");
    /*测试数据  硬写入一组卡片*/
    system_card_pool.card_pool[0].card_id = 0;
    system_card_pool.card_pool[0].card_name="bubu";
    system_card_pool.card_pool[0].card_type="CN";
    system_card_pool.card_pool[0].card_level=LEVEL_5;
    system_card_pool.card_pool[0].card_description="block yang's cat";
    system_card_pool.card_pool[0].is_fetched_card=false;
    system_card_pool.card_pool[0].is_user_card=false;
    system_card_pool.card_count++;
    ESP_LOGI(CARDPOOL,"cardpool add card %d",system_card_pool.card_pool[0].card_id);
    ESP_LOGI(CARDPOOL,"cardpool add card count %d",system_card_pool.card_count);
#endif


}

void system_list_out_pool_card(void)
{
    /*检查系统卡池是否为空*/
    if (system_card_pool.card_count == 0){
        /*log error*/
        ESP_LOGE(CARDPOOL,"cardpool is empty");
        return;
    }
    /*遍历卡组并打印卡牌*/
    for(int i=0;i<MAX_CARDS;i++)
    {
        if(system_card_pool.card_pool[i].card_name!=NULL) {
            /*log info*/
            ESP_LOGI(CARDPOOL,"cardpool card %d %s %s %d %s %s %d %d",system_card_pool.card_pool[i].card_id,
                                            system_card_pool.card_pool[i].card_name,
                                            system_card_pool.card_pool[i].card_type,
                                            system_card_pool.card_pool[i].card_level,
                                            system_card_pool.card_pool[i].card_description,
                                            system_card_pool.card_pool[i].is_fetched_card,
                                            system_card_pool.card_pool[i].is_user_card);
        }
        else {
            /*log error*/
            ESP_LOGE(CARDPOOL,"cardpool card %d is empty",i);
            break;
        }
    }

}

bool add_card(card_pool_t* pool, card_t* card) {
    /*检查卡池是否为空*/
    if (pool == NULL || card == NULL) {
        /*log error*/
        return false;
    }
    /*检查卡池是否为用户卡池*/
    if (strcmp(pool->card_pool_name,"user_card_pool") == 0) {
        /*找出卡池中空闲的位置*/
        /*遍历卡组*/
        for (int i=0; i < MAX_CARDS; i++){
            if(pool->card_pool[i].card_name == NULL ) {
                /*memcpy*/
                /*逐个复制*/
            }
        }
    }
    else{
        /*log error*/
        return false;
    }
    /*log info*/
    return true;
}

// 删除卡片
bool remove_card(card_pool_t* pool, int card_id) {
    /*检查card_id对应的卡是否是空*/
    /*检查卡池合法性*/
    int id = card_id - 1;
    if (pool == NULL || (id) < 0 || (id) >= MAX_CARDS) {
        /*log error*/
        return false;
    }
    if (pool->card_pool[id].card_name == NULL) {
        /*log error*/
        return false;
    }
    pool->card_pool[id].card_name = NULL;
    pool->card_pool[id].card_description = NULL;
    pool->card_pool[id].card_type = NULL;
    pool->card_pool[id].card_level = 0;
   //pool->card_pool[id].is_fetched_card = false;
   //pool->card_pool[id].is_user_card = false;

    return true;
}

// 查找卡片
card_t* find_card(card_pool_t* pool, int card_id) {
    int id = card_id - 1;
    /*检查card_id对应的卡是否是空*/
    if (pool == NULL || (id) < 0 || (id) >= MAX_CARDS) {
        /*log error*/
        return NULL;
    }
    if (pool->card_pool[id].card_name == NULL) {
        /*log error*/
        return NULL;
    }
    /*打印卡牌信息*/
    /*log info*/
    return &pool->card_pool[id];

}

// 更新卡片
bool update_card(card_pool_t* pool, int card_id, card_t* new_card) {
    int id = card_id - 1;
    if (pool == NULL || new_card == NULL || (id) < 0 || (id) >= MAX_CARDS) {
        /*log error*/
        return false;
    }
    if (pool->card_pool[id].card_name == NULL) {
        /*log error*/
        return false;
    }
    pool->card_pool[id].card_name= new_card->card_name;
    pool->card_pool[id].card_description= new_card->card_description;
    pool->card_pool[id].card_type= new_card->card_type;
    pool->card_pool[id].card_level= new_card->card_level;

    /*打印新卡牌的信息*/
    /*log info*/
    return true;
}