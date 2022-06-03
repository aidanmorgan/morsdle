//
// Created by Aidan Morgan on 3/6/2022.
//

#ifndef MORSDLE_WAVESHARE_SPI_H
#define MORSDLE_WAVESHARE_SPI_H

#include <stdint.h>

// Pins on board:
//  VCC: 3.3v
//  GND: Ground
//  DIN: SPI MOSI Pin (micro to slave in)
//  CLK: SPI SCK Pin
//  CS : SPI Chip Select, Low Active
//  DC : Data or Command Select (high data, low command)
//  RST: External Reset - low active
//  BUSY: Busy status (display to micro)
extern void wavesharespi_write_dc(uint8_t val);
extern void wavesharespi_write_cs(uint8_t val);
extern void wavesharespi_write_rst(uint8_t val);
extern void wavesharespi_write_spi(uint8_t val);
extern void wavesharespi_read_busy(uint8_t* val);
extern void wavesharespi_delay(uint32_t time);

#endif //MORSDLE_WAVESHARE_SPI_H
