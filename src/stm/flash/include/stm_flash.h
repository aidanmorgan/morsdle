//
// Created by Aidan Morgan on 13/6/2022.
//

#ifndef MORSDLE_MORSDLE_FLASH_H
#define MORSDLE_MORSDLE_FLASH_H

#include <stdint.h>
#include <stddef.h>
#include "main.h"

typedef enum {
    FLASH_OK  = 0,
    FLASH_METADATALOAD_FAILURE,
    FLASH_METADATA_MISSINGFONT,
    FLASH_METADATA_NODICTIONARY
} flash_error_t;

typedef struct {
    uint8_t ch;
    size_t size;
} flash_font_metadata_t;

typedef struct {
    uint32_t word_count;
    // contains an array of font metadata information, sorted in alphabetical order to allow an immediate lookup
    flash_font_metadata_t fonts [26];
} flash_metadata_t;

typedef struct {
    QSPI_HandleTypeDef* h_qspi;

    uint32_t metadata_region_offset;
    size_t metadata_region_size;

    uint32_t dictionary_region_offset;
    size_t dictionary_region_size;

    uint32_t fonts_region_offset;
    size_t fonts_region_size;
} flash_cfg_t;

flash_error_t flash_read_metadata(flash_cfg_t* cfg, flash_metadata_t* meta);

#endif //MORSDLE_MORSDLE_FLASH_H
