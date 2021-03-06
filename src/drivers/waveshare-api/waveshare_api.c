//
// Created by Aidan Morgan on 27/5/2022.
//

#include <assert.h>
#include "waveshare_api.h"
#include "waveshare_spi.h"

static uint8_t buffer_to_waveshare_lookup[4] = {
        WAVESHARE_WHITE,  // clear
        WAVESHARE_ORANGE,// orange
        WAVESHARE_GREEN, // green
        WAVESHARE_BLACK  // black
};


inline waveshareapi_error waveshareapi_moduleinit(void) {
    wavesharespi_write_dc(0);
    wavesharespi_write_cs(0);
    wavesharespi_write_rst(1);

    return WAVESHAREAPI_OK;
}

inline void waveshareapi_moduleexit(void) {
    wavesharespi_write_dc(0);
    wavesharespi_write_cs(0);
    wavesharespi_write_rst(0);
}


static void waveshareapi_wait_busyhigh(void) {
    while(!(wavesharespi_read_busy()));
}

static void waveshareapi_wait_busylow(void) {
    while((wavesharespi_read_busy()));
}

void waveshareapi_send_data(uint8_t data) {
    wavesharespi_write_dc(1);
    wavesharespi_write_cs(0);
    wavesharespi_write_spi(data);
    wavesharespi_write_cs(1);
}

void waveshareapi_send_command(uint8_t data) {
    wavesharespi_write_dc(0);
    wavesharespi_write_cs(0);
    wavesharespi_write_spi(data);
    wavesharespi_write_cs(1);
}

void waveshareapi_reset(void) {
    wavesharespi_write_rst(1);
    wavesharespi_delay(200);
    wavesharespi_write_rst(0);
    wavesharespi_delay(1);
    wavesharespi_write_rst(1);
    wavesharespi_delay(200);
}

void waveshareapi_init(void) {
//    waveshareapi_reset();

    waveshareapi_wait_busyhigh();
    waveshareapi_send_command(0x00);
    waveshareapi_send_data(0xEF);
    waveshareapi_send_data(0x08);
    waveshareapi_send_command(0x01);
    waveshareapi_send_data(0x37);
    waveshareapi_send_data(0x00);
    waveshareapi_send_data(0x23);
    waveshareapi_send_data(0x23);
    waveshareapi_send_command(0x03);
    waveshareapi_send_data(0x00);
    waveshareapi_send_command(0x06);
    waveshareapi_send_data(0xC7);
    waveshareapi_send_data(0xC7);
    waveshareapi_send_data(0x1D);
    waveshareapi_send_command(0x30);
    waveshareapi_send_data(0x3C);
    waveshareapi_send_command(0x41);
    waveshareapi_send_data(0x00);
    waveshareapi_send_command(0x50);
    waveshareapi_send_data(0x37);
    waveshareapi_send_command(0x60);
    waveshareapi_send_data(0x22);
    waveshareapi_send_command(0x61);
    waveshareapi_send_data(0x02);
    waveshareapi_send_data(0x58);
    waveshareapi_send_data(0x01);
    waveshareapi_send_data(0xC0);
    waveshareapi_send_command(0xE3);
    waveshareapi_send_data(0xAA);

    wavesharespi_delay(100);
    waveshareapi_send_command(0x50);
    waveshareapi_send_data(0x37);
}

void waveshareapi_destroy(void) {

}

#define EPD_5IN65F_HEIGHT 448
#define EPD_5IN65F_WIDTH 600

 void waveshareapi_render_region(imagebuffer_t *buffer, uint16_t xstart, uint16_t ystart, uint16_t xend, uint16_t yend,
                                uint16_t rotation) {
    uint64_t i, j;

    // The waveshare device encodes two colours to a byte in the width direction, so we need to always make sure
    // we are reading an even number of colors
    if (xstart % 2 != 0) {
        xstart += 1;
    }

    if (xend % 2 != 0) {
        xend += 1;
    }

    uint16_t region_width = xend - xstart;
    uint16_t region_height = yend - ystart;

    waveshareapi_send_command(0x61);//Set Resolution setting
    waveshareapi_send_data(0x02);
    waveshareapi_send_data(0x58);
    waveshareapi_send_data(0x01);
    waveshareapi_send_data(0xC0);
    waveshareapi_send_command(0x10);

    colour_t left = COLOUR_INVALID;
    colour_t right = COLOUR_INVALID;
    uint8_t data;

    // step through in twos in the x-direction, because we're going to read out two values in one go as the waveshare
    // device encodes two pixels into each horizontal only, whereas I encode 2 in both directions (because im using less colours).
    for (i = 0; i < EPD_5IN65F_HEIGHT; i++) {
        for (j = 0; j < EPD_5IN65F_WIDTH; j += 2) {
            if(rotation == 0) {
                imagebuffer_getpixel(buffer, j, i, &left);
                imagebuffer_getpixel(buffer, j + 1, i, &right);
            } else if (rotation == 90) {
                imagebuffer_getpixel(buffer,  i, EPD_5IN65F_WIDTH - j, &left);
                imagebuffer_getpixel(buffer, i, EPD_5IN65F_WIDTH - j - 1, &right);
            }
            else if(rotation == 180) {
                imagebuffer_getpixel(buffer,  EPD_5IN65F_WIDTH - j, EPD_5IN65F_HEIGHT - i, &left);
                imagebuffer_getpixel(buffer, EPD_5IN65F_WIDTH - j + 1, EPD_5IN65F_HEIGHT - i, &right);
            }
            else if(rotation == 270) {
                imagebuffer_getpixel(buffer,  i - 1, EPD_5IN65F_WIDTH - j, &left);
                imagebuffer_getpixel(buffer, i - 1, EPD_5IN65F_WIDTH - j - 1, &right);
            }

            data = (buffer_to_waveshare_lookup[left] << 4) | buffer_to_waveshare_lookup[right];
            waveshareapi_send_data(data);
        }
    }

    waveshareapi_send_command(0x04);//0x04
    waveshareapi_wait_busyhigh();
    waveshareapi_send_command(0x12);//0x12
    waveshareapi_wait_busyhigh();
    waveshareapi_send_command(0x02);  //0x02
    waveshareapi_wait_busylow();
    wavesharespi_delay(200);
}

void waveshareapi_clear(uint8_t colour) {
    waveshareapi_send_command(0x61);//Set Resolution setting
    waveshareapi_send_data(0x02);
    waveshareapi_send_data(0x58);
    waveshareapi_send_data(0x01);
    waveshareapi_send_data(0xC0);
    waveshareapi_send_command(0x10);

    // we can loop over the phusical constraints here because it's a clear operation, we don't actually care about the
    // rotation at the moment.
    for (int i = 0; i < EPD_5IN65F_WIDTH / 2; i++) {
        for (int j = 0; j < EPD_5IN65F_HEIGHT; j++)
            waveshareapi_send_data((colour << 4) | colour);
    }

    waveshareapi_send_command(0x04);//0x04
    waveshareapi_wait_busyhigh();
    waveshareapi_send_command(0x12);//0x12
    waveshareapi_wait_busyhigh();
    waveshareapi_send_command(0x02);  //0x02
    waveshareapi_wait_busylow();
    wavesharespi_delay(500);
}


