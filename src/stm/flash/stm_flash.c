//
// Created by Aidan Morgan on 13/6/2022.
//

#include "stm_flash_read.h"
#include "stm_flash_write.h"

flash_error_t flash_read_metadata(flash_cfg_t* cfg, flash_metadata_t* meta) {
    return FLASH_OK;
}

flash_error_t flash_read_word(flash_cfg_t* cfg, flash_metadata_t* meta, uint32_t index, uint8_t* result) {
    return FLASH_OK;
}

flash_error_t flash_read_font(flash_cfg_t*cfg, flash_metadata_t* meta, uint8_t letter, uint8_t* data) {
    return FLASH_OK;
}

flash_error_t flash_init_session(flash_cfg_t* cfg, flash_write_session_t* session) {
    return FLASH_OK;
}

flash_error_t flash_erase_words(flash_write_session_t* cfg) {
    return FLASH_OK;
}

flash_error_t flash_erase_fonts(flash_write_session_t* cfg){
    return FLASH_OK;
}

flash_error_t flash_add_word(flash_write_session_t* cfg, uint8_t* word){
    return FLASH_OK;
}

flash_error_t flash_add_font(flash_write_session_t* cfg, uint8_t letter, uint8_t* word){
    return FLASH_OK;
}

flash_error_t flash_flush(flash_write_session_t* session){
    return FLASH_OK;
}

