//
// Created by Aidan Morgan on 9/6/2022.
//

#ifndef MORSDLE_MORSDLE_MAIN_H
#define MORSDLE_MORSDLE_MAIN_H

#include "main.h"
#include "debug.h"

typedef enum {
    GAME_MODE,
    LOADER_MODE
} stm_mode_t;

// for callbacks that are shared between the loader and the game loop, this is populated when they are
// started with function pointers that the top-level code will then call through
typedef struct {

} stm_delegates_t;

extern void game_main(stm32_config_t*, stm_delegates_t*);
extern void loader_main(stm32_config_t*, stm_delegates_t*);

#endif //MORSDLE_MORSDLE_MAIN_H
