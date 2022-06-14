#include "morsdle_main.h"
#include "stm_flash_write.h"

// commands:
// FONT <letter:1> <data_size:uint32_t> then data
// WORD <letter-count:1> <5-letter string>
// chosen so any read from the UART can just read the first 6 characters and then know what to do making
// building the system a little easier
void loader_main(stm32_config_t* config, flash_cfg_t* flashcfg) {
    while(1) {

    }
}