//
// Created by Aidan Morgan on 27/5/2022.
//

#include "waveshare_api.h"
#include "waveshare_spi.h"


void waveshareapi_wake(void)
{
    wavesharespi_write_dc(0);
    wavesharespi_write_cs(0);
    wavesharespi_write_rst(1);
}

void waveshareapi_sleep(void)
{
    wavesharespi_write_dc(0);
    wavesharespi_write_cs(0);
    wavesharespi_write_rst(0);
}

static void inline waveshareapi_waitbusy_high() {
    uint8_t data = 1;
    while (!data) {
        wavesharespi_read_busy(&data);
    }
}

static void inline waveshareapi_waitbusy_low() {
    uint8_t data = 0;
    while (data) {
        wavesharespi_read_busy(&data);
    }
}

void waveshareapi_send_data(uint8_t data) {
    wavesharespi_write_dc(1);
    wavesharespi_write_cs(0);
    wavesharespi_write_spi(data);
    wavesharespi_write_cs(1);
}

void waveshareapi_send_command(uint8_t data) {
    wavesharespi_write_dc(0);
    wavesharespi_write_dc(0);
    wavesharespi_write_spi(data);
    wavesharespi_write_dc(1);
}

void waveshareapi_reset() {
    wavesharespi_write_rst(1);
    wavesharespi_delay(200);
    wavesharespi_write_rst(0);
    wavesharespi_delay(1);
    wavesharespi_write_rst(1);
    wavesharespi_delay(200);
}

void waveshareapi_init(imagebuffer_t * buffer) {
    waveshareapi_reset();

    waveshareapi_waitbusy_high();
    waveshareapi_send_command(0x00);
    waveshareapi_send_data( 0xEF);
    waveshareapi_send_data( 0x08);
    waveshareapi_send_command( 0x01);
    waveshareapi_send_data( 0x37);
    waveshareapi_send_data( 0x00);
    waveshareapi_send_data( 0x23);
    waveshareapi_send_data( 0x23);
    waveshareapi_send_command( 0x03);
    waveshareapi_send_data( 0x00);
    waveshareapi_send_command( 0x06);
    waveshareapi_send_data( 0xC7);
    waveshareapi_send_data( 0xC7);
    waveshareapi_send_data( 0x1D);
    waveshareapi_send_command( 0x30);
    waveshareapi_send_data( 0x3C);
    waveshareapi_send_command( 0x41);
    waveshareapi_send_data( 0x00);
    waveshareapi_send_command( 0x50);
    waveshareapi_send_data( 0x37);
    waveshareapi_send_command( 0x60);
    waveshareapi_send_data( 0x22);
    waveshareapi_send_command( 0x61);
    waveshareapi_send_data( 0x02);
    waveshareapi_send_data( 0x58);
    waveshareapi_send_data( 0x01);
    waveshareapi_send_data( 0xC0);
    waveshareapi_send_command( 0xE3);
    waveshareapi_send_data( 0xAA);

    wavesharespi_delay(100);
    waveshareapi_send_command( 0x50);
    waveshareapi_send_data( 0x37);
}

void waveshareapi_destroy(imagebuffer_t * buffer) {

}

#define EPD_5IN65F_HEIGHT 448
#define EPD_5IN65F_WIDTH 600

void waveshareapi_render_region(imagebuffer_t* buffer, uint16_t width, uint16_t height, uint16_t xstart, uint16_t ystart, uint16_t xend, uint16_t yend) {
    uint64_t i,j;
    waveshareapi_send_command(0x61);//Set Resolution setting
    waveshareapi_send_data(0x02);
    waveshareapi_send_data(0x58);
    waveshareapi_send_data(0x01);
    waveshareapi_send_data(0xC0);
    waveshareapi_send_command(0x10);

    // The waveshare device encodes to colours to a byte in the width direction, so we need to always make sure
    // we are reading an even number of colors
    if(xstart % 2 != 0) {
        xstart -= 1;
    }

    if(xend %2 != 0) {
        xend += 1;
    }

    for(i=0; i<buffer->height; i++) {
        // step through in twos, because we're going to read out two values in one go as the waveshare device encodes two
        // pixels into each horizontal
        for(j=0; j<buffer->width; j+=2) {
            imagebuffer_colour_t color[2] = {IMAGEBUFFER_INVALID, IMAGEBUFFER_INVALID};

            imagebuffer_getpixel(buffer, j, i, &color[0]);
            imagebuffer_getpixel(buffer, j + 1, i, &color[1]);

            uint8_t data = buffer_to_waveshare_lookup[color[0]] << 4 | buffer_to_waveshare_lookup[color[1]];
            waveshareapi_send_data(data);
        }
    }
    waveshareapi_send_command(0x04);//0x04
    waveshareapi_waitbusy_high();
    waveshareapi_send_command(0x12);//0x12
    waveshareapi_waitbusy_high();
    waveshareapi_send_command(0x02);  //0x02
    waveshareapi_waitbusy_low();
    wavesharespi_delay(200);
}


