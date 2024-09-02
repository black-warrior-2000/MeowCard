#ifndef CARDPOOL_H
#define CARDPOOL_H

#endif

/* includes */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*macros */

#define MAX_CARDS 100
#define POOL_DEEPTH 100
/*card pool data structure */

typedef enum card_type_e
{
    MONSTER,
    SPELL,
    TRAP
} card_type_t;

typedef enum card_level_e
{
    LEVEL_1,
    LEVEL_2,
    LEVEL_3,
    LEVEL_4,
    LEVEL_5,
} card_level_t;

typedef struct card_s
{
    bool is_user_card;
    bool is_fetched_card;
    int card_count;
    int card_id;
    int card_level;
    char* card_name;
    char* card_type;
    char* card_description;
} card_t;

typedef struct card_pool_s
{
    card_t card_pool[MAX_CARDS];
    char *card_pool_name;
    uint64_t card_count;
} card_pool_t;

/*用户卡池和总卡池*/


/*internal function prototypes */

static void card_pool_init(void);


/*external function prototypes */

void system_card_pool_init(void);