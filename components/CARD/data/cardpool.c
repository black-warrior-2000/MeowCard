#include "cardpool.h"


card_pool_t system_card_pool={};
card_pool_t user_card_pool={};


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
    
    system_card_pool.card_pool[0].card_id = 0;
    system_card_pool.card_pool[0].card_name="bubu";
    system_card_pool.card_pool[0].card_type="CN",
    system_card_pool.card_pool[0].card_level=LEVEL_5,
    system_card_pool.card_pool[0].card_description="block yang's cat",
    system_card_pool.card_pool[0].is_fetched_card=false,
    system_card_pool.card_pool[0].is_user_card=false;
    


}

bool add_card(card_pool_t* pool, card_t* card) {
    return true;
}

// 删除卡片
bool remove_card(card_pool_t* pool, int card_id) {
    return true;
}

// 查找卡片
card_t* find_card(card_pool_t* pool, int card_id) {
    return NULL;
}

// 更新卡片
bool update_card(card_pool_t* pool, int card_id, card_t* new_card) {
    return true;
}