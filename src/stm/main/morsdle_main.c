
#include "morsdle_main.h"
#include "waveshare_spi_impl.h"
#include "waveshare_api.h"
#include "stm_flash.h"

static flash_cfg_t h_flashcfg = (flash_cfg_t) {};

static stm_mode_t mode = GAME_MODE;

void flash_init(flash_cfg_t* flashcfg) {
    flashcfg->h_qspi = &hqspi;
}

int main(void) {
    init_stm32l476();

    console_t console = (console_t) {
        .uart = &huart4
    };

    console_init(&console);
    flash_init(&h_flashcfg);

    // if this pin is high (by applying a jumper on the board) we will drop into a programming mode
    // for programming the external flash with new icons/dictionary entries via UART rather than
    // playing the game.
    if(HAL_GPIO_ReadPin(StartupMode_GPIO_Port, StartupMode_Pin)) {
        mode = LOADER_MODE;
        loader_main( &h_flashcfg, &console);
    }
    else {
        mode = GAME_MODE;
        game_main( &h_flashcfg, &console);
    }

    return 0;
}




