//
// Created by Aidan Morgan on 9/6/2022.
//

#ifndef MORSDLE_MORSDLE_MAIN_H
#define MORSDLE_MORSDLE_MAIN_H

#include "main.h"
#include "stm_stdio.h"
#include "stm_flash.h"

typedef enum {
    GAME_MODE,
    LOADER_MODE
} stm_mode_t;

extern UART_HandleTypeDef huart4;
extern  QSPI_HandleTypeDef hqspi;
extern RNG_HandleTypeDef hrng;


extern void game_main(flash_cfg_t*, console_t*);
extern void loader_main(flash_cfg_t*, console_t*);

#endif //MORSDLE_MORSDLE_MAIN_H
