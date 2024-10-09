#ifndef __DRAW_CARD_H__
#define __DRAW_CARD_H__

#include "cardpool.h"

#define MAX_CARDS 100
#define MIN_CARDS 1
#define POOL_DEEPTH 100

typedef struct weight_of_card
{
    /* data */
    uint32_t  card_level;
    uint32_t  card_level_weight;
} weight_of_card_t;



extern uint32_t draw_card(weight_of_card_t * weight, int len);




#endif