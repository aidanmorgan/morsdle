
#include "morsdle_main.h"
#include "stm_flash.h"

// passed into the init board function to get access to the static fields for use elsewhere in this
// main loop because the STM32CubeMX tool generates the handles to specific hardware as static instances
// to annoy me even when the "generate main" option is disabled.
static stm32_config_t h_hwconfig = (stm32_config_t){ };
static flash_cfg_t h_flashcfg = (flash_cfg_t) {};

static stm_mode_t mode = GAME_MODE;

void flash_init(stm32_config_t* stmcfg, flash_cfg_t* flashcfg) {
    flashcfg->h_qspi = stmcfg->flash_qspi;
}

int main(void) {
    init_stm_board(&h_hwconfig);    // implemented by hand in the generated code to perform initialisation

#ifdef DEBUG
    debug_init(&h_hwconfig);
#endif

    flash_init(&h_hwconfig, &h_flashcfg);

    // if this pin is high (by applying a jumper on the board) we will drop into a programming mode
    // for programming the external flash with new icons/dictionary entries via UART rather than
    // playing the game.
    if(HAL_GPIO_ReadPin(h_hwconfig.startup_mode_port, h_hwconfig.startup_mode_pin)) {
        mode = LOADER_MODE;
        loader_main(&h_hwconfig, &h_flashcfg);
    }
    else {
        mode = GAME_MODE;
        game_main(&h_hwconfig, &h_flashcfg);
    }

    return 0;
}




