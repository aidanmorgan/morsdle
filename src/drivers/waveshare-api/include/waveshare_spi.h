//
// Created by Aidan Morgan on 20/6/2022.
//

#ifndef MORSDLE_WAVESHARE_SPI_H
#define MORSDLE_WAVESHARE_SPI_H

#include <stdint.h>

extern void wavesharespi_write_dc(uint8_t  val);
extern void wavesharespi_write_cs(uint8_t val);
extern void wavesharespi_write_rst(uint8_t val);
extern void wavesharespi_write_spi(uint8_t val);
extern uint8_t wavesharespi_read_busy();
extern void wavesharespi_delay(uint32_t time);
extern uint32_t wavesharespi_ticks();


#endif //MORSDLE_WAVESHARE_SPI_H
