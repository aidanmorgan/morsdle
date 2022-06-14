//
// Created by Aidan Morgan on 13/6/2022.
//

#ifndef MORSDLE_MORSDLE_FLASH_WRITE_H
#define MORSDLE_MORSDLE_FLASH_WRITE_H

#include "stm_flash.h"

typedef struct {
    flash_cfg_t* flash_cfg;

    uint32_t word_tail_idx;
    uint32_t word_head_idx;

    uint32_t icon_tail_idx;
    uint32_t icon_head_idx;
} flash_write_session_t;

flash_error_t flash_init_session(flash_cfg_t* cfg, flash_write_session_t* session);
flash_error_t flash_erase_words(flash_write_session_t* cfg);
flash_error_t flash_erase_fonts(flash_write_session_t* cfg);

flash_error_t flash_add_word(flash_write_session_t* cfg, uint8_t* word);
flash_error_t flash_add_font(flash_write_session_t* cfg, uint8_t letter, uint8_t* word);

flash_error_t flash_flush(flash_write_session_t* session);


#endif //MORSDLE_MORSDLE_FLASH_WRITE_H
