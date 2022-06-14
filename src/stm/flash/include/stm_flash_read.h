//
// Created by Aidan Morgan on 13/6/2022.
//

#ifndef MORSDLE_MORSDLE_FLASH_READ_H
#define MORSDLE_MORSDLE_FLASH_READ_H

#include "stm_flash.h"

flash_error_t flash_read_word(flash_cfg_t* cfg, flash_metadata_t* meta, uint32_t index, uint8_t* result);
flash_error_t flash_read_font(flash_cfg_t*cfg, flash_metadata_t* meta, uint8_t letter, uint8_t* data);

#endif //MORSDLE_MORSDLE_FLASH_READ_H
