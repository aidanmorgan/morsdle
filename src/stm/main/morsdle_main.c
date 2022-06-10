
#include "morsdle_main.h"

// passed into the init board function to get access to the static fields for use elsewhere in this
// main loop because the STM32CubeMX tool generates the handles to specific hardware as static instances
// to annoy me even when the "generate main" option is disabled.
static stm32_config_t hw_config;


static stm_mode_t mode = GAME_MODE;
static stm_delegates_t delegates = {};

int main(void) {
    init_stm_board(&hw_config);    // implemented by hand in the generated code to perform initialisation

#ifdef DEBUG
    debug_init(&hw_config);
#endif

    // if this pin is high (by applying a jumper on the board) we will drop into a programming mode
    // for programming the external flash with new icons/dictionary entries via UART rather than
    // playing the game.
    if(HAL_GPIO_ReadPin(hw_config.startup_mode_port, hw_config.startup_mode_pin)) {
        mode = LOADER_MODE;
        loader_main(&hw_config, &delegates);
    }
    else {
        mode = GAME_MODE;
        game_main(&hw_config, &delegates);
    }

    return 0;
}



