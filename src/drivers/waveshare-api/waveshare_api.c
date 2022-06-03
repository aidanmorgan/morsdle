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

void waveshareapi_clear(uint16_t width, uint16_t height, uint8_t color) {
    waveshareapi_send_command( 0x61);//Set Resolution setting
    waveshareapi_send_data( 0x02);
    waveshareapi_send_data( 0x58);
    waveshareapi_send_data( 0x01);
    waveshareapi_send_data( 0xC0);
    waveshareapi_send_command( 0x10);

    for (int i = 0; i < width / 2; i++) {
        for (int j = 0; j < height; j++)
            waveshareapi_send_data( (color << 4) | color);
    }

    waveshareapi_send_command( 0x04);//0x04
    waveshareapi_waitbusy_high();
    waveshareapi_send_command( 0x12);//0x12
    waveshareapi_waitbusy_high();
    waveshareapi_send_command( 0x02);  //0x02
    waveshareapi_waitbusy_low();
    wavesharespi_delay(500);
}


void waveshareapi_init() {
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

void waveshareapi_destroy() {

}

void waveshareapi_render_region(uint8_t* buffer, uint16_t width, uint16_t height, uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end) {
    uint64_t i,j;

    uint16_t region_width = x_end - x_start;
    uint16_t region_height = y_end - y_start;

    waveshareapi_send_command(0x61);//Set Resolution setting
    waveshareapi_send_data(0x02);
    waveshareapi_send_data(0x58);
    waveshareapi_send_data(0x01);
    waveshareapi_send_data(0xC0);
    waveshareapi_send_command(0x10);

    for(i=0; i<height; i++) {
        for(j=0; j< width/2; j++) {
            if(i<region_height+y_start && i>=y_start && j<(region_width+x_start)/2 && j>=x_start/2) {
                waveshareapi_send_data(buffer[(j-x_start/2) + (region_width/2*(i-y_start))]);
            }
            else {
                waveshareapi_send_data(0x11);
            }
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
