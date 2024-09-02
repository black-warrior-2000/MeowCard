#include "drawcard.h"
#include <esp_random.h>

 uint32_t  weight[5] = {5,10,15,20,50};
 uint32_t  sum_weight = POOL_DEEPTH;//

//#define TIME_RANDOM 1
#define ESP_RANDOM 1

typedef struct weight_of_card
{
    /* data */
    uint32_t  card_level;
    uint32_t  card_level_weight;
} weight_of_card_t;


weight_of_card_t normal_weight[5] = {
    {1,5},
    {2,10},
    {3,15},
    {4,20},
    {5,50}
};

int32_t draw_card(weight_of_card_t * weight, int len);

int32_t draw_card(weight_of_card_t * weight, int len){
    /*获取随机数*/
    uint32_t random_num;
    uint32_t range = MAX_CARDS - MIN_CARDS + 1;

    /*检查weight 指针合法性*/
    if(weight == NULL)
    {
        /*log error*/
        return -1;
    }
    /*
    weight_of_card_t tmp_weight[5] = {0};
    for(int i = 0; i < len; i++)
    {
        tmp_weight[i].card_level = weight[i].card_level;
        tmp_weight[i].card_level_weight = weight[i].card_level_weight;
    }
    */
/* 
#ifdef ESP_RANDOM{
    do {
        random_num = esp_random();
    } while (random_num >= UINT32_MAX - （UINT32_MAX % range）);
    random_num = random_num % range + MIN_CARDS;
}
#endif
*/

    /*计算累积权重*/
    int total_weight[5] = {0};
    total_weight[0] = weight[0].card_level_weight;
    
    for(int i = 1; i < 5; i++)
    {
        total_weight[i] = total_weight[i-1] + weight[i].card_level_weight;// key 
    }

    /*获取随机数*/
#ifdef ESP_RANDOM
    do {
        random_num = esp_random();
    } while (random_num >= UINT32_MAX - ( UINT32_MAX % range ));
    random_num = random_num % range + MIN_CARDS;
#endif

    /*查找对应卡牌*/


/*
    for (int i = 0; i < 5; i++) {
        total_weight += normal_weight[i].card_level_weight;
    }

    //计算累积权重
    int cum_weight[5]={0};
    int tmp = 0;
    tmp =  normal_weight[0].card_level_weight;
    cum_weight[0] =tmp;
    for (int i = 1; i < 5; i++) {
        cum_weight[i] = cum_weight[i-1] + normal_weight[i].card_level_weight;
    }
    
*/
    return random_num;
}











