//
// Created by Aidan Morgan on 9/6/2022.
//

#ifndef MORSDLE_MORSDLE_MAIN_H
#define MORSDLE_MORSDLE_MAIN_H

#include "main.h"
#include "debug.h"
#include "stm_flash.h"

typedef enum {
    GAME_MODE,
    LOADER_MODE
} stm_mode_t;

extern void game_main(stm32_config_t*, flash_cfg_t*);
extern void loader_main(stm32_config_t*, flash_cfg_t*);

#endif //MORSDLE_MORSDLE_MAIN_H
