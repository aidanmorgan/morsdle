//
// Created by Aidan Morgan on 27/5/2022.
//

#include "waveshare_stm.h"


static void inline wavesharestm_digital_write(wavesharestm_pin_t request, uint8_t val) {
    HAL_GPIO_WritePin(request.port, request.pin, val == 0 ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

static void inline wavesharestm_digital_read(wavesharestm_pin_t request, uint8_t *result) {
    *result = HAL_GPIO_ReadPin(request.port, request.pin);
}

void wavesharestm_wake(void)
{
    wavesharestm_digital_write(WAVESHARESTM_DC_PIN, 0);
    wavesharestm_digital_write(WAVESHARESTM_CS_PIN, 0);
    wavesharestm_digital_write(WAVESHARESTM_RST_PIN, 1);
}

void wavesharestm_sleep(void)
{
    wavesharestm_digital_write(WAVESHARESTM_DC_PIN, 0);
    wavesharestm_digital_write(WAVESHARESTM_CS_PIN, 0);

    //close 5V
    wavesharestm_digital_write(WAVESHARESTM_RST_PIN, 0);
}


static void inline wavesharestm_spi_write(uint8_t data) {
    HAL_SPI_Transmit(WAVESHARE_SPI_HANDLE, &data, 1, 1000);
}

static void inline wavesharestm_waitbusy_high() {
    uint8_t data = 1;
    while (!data) {
        wavesharestm_digital_read(WAVESHARESTM_BUSY_PIN, &data);
    }
}

static void inline wavesharestm_waitbusy_low() {
    uint8_t data = 0;
    while (data) {
        wavesharestm_digital_read(WAVESHARESTM_BUSY_PIN, &data);
    }
}

void wavesharestm_send_data(uint8_t data) {
    wavesharestm_digital_write(WAVESHARESTM_DC_PIN, 1);
    wavesharestm_digital_write(WAVESHARESTM_CS_PIN, 0);
    wavesharestm_spi_write( data);
    wavesharestm_digital_write(WAVESHARESTM_CS_PIN, 1);
}

void wavesharestm_send_command(uint8_t data) {
    wavesharestm_digital_write(WAVESHARESTM_DC_PIN, 0);
    wavesharestm_digital_write(WAVESHARESTM_DC_PIN, 0);
    wavesharestm_spi_write( data);
    wavesharestm_digital_write(WAVESHARESTM_DC_PIN, 1);
}

void wavesharestm_delay(uint32_t ms) {
    HAL_Delay(ms);
}

void wavesharestm_reset() {
    wavesharestm_digital_write(WAVESHARESTM_RST_PIN, 1);
    wavesharestm_delay( 200);
    wavesharestm_digital_write(WAVESHARESTM_RST_PIN, 0);
    wavesharestm_delay( 1);
    wavesharestm_digital_write(WAVESHARESTM_RST_PIN, 1);
    wavesharestm_delay( 200);
}

void wavesharestm_clear(uint16_t width, uint16_t height, uint8_t color) {
    wavesharestm_send_command( 0x61);//Set Resolution setting
    wavesharestm_send_data( 0x02);
    wavesharestm_send_data( 0x58);
    wavesharestm_send_data( 0x01);
    wavesharestm_send_data( 0xC0);
    wavesharestm_send_command( 0x10);

    for (int i = 0; i < width / 2; i++) {
        for (int j = 0; j < height; j++)
            wavesharestm_send_data( (color << 4) | color);
    }

    wavesharestm_send_command( 0x04);//0x04
    wavesharestm_waitbusy_high();
    wavesharestm_send_command( 0x12);//0x12
    wavesharestm_waitbusy_high();
    wavesharestm_send_command( 0x02);  //0x02
    wavesharestm_waitbusy_low();
    wavesharestm_delay( 500);
}


void wavesharestm_init() {
    wavesharestm_reset();

    wavesharestm_waitbusy_high();
    wavesharestm_send_command(0x00);
    wavesharestm_send_data( 0xEF);
    wavesharestm_send_data( 0x08);
    wavesharestm_send_command( 0x01);
    wavesharestm_send_data( 0x37);
    wavesharestm_send_data( 0x00);
    wavesharestm_send_data( 0x23);
    wavesharestm_send_data( 0x23);
    wavesharestm_send_command( 0x03);
    wavesharestm_send_data( 0x00);
    wavesharestm_send_command( 0x06);
    wavesharestm_send_data( 0xC7);
    wavesharestm_send_data( 0xC7);
    wavesharestm_send_data( 0x1D);
    wavesharestm_send_command( 0x30);
    wavesharestm_send_data( 0x3C);
    wavesharestm_send_command( 0x41);
    wavesharestm_send_data( 0x00);
    wavesharestm_send_command( 0x50);
    wavesharestm_send_data( 0x37);
    wavesharestm_send_command( 0x60);
    wavesharestm_send_data( 0x22);
    wavesharestm_send_command( 0x61);
    wavesharestm_send_data( 0x02);
    wavesharestm_send_data( 0x58);
    wavesharestm_send_data( 0x01);
    wavesharestm_send_data( 0xC0);
    wavesharestm_send_command( 0xE3);
    wavesharestm_send_data( 0xAA);

    wavesharestm_delay( 100);
    wavesharestm_send_command( 0x50);
    wavesharestm_send_data( 0x37);
}

void wavesharestm_destroy() {

}

void wavesharestm_render_impl(uint8_t* buffer, uint16_t width, uint16_t height, uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end) {
    uint64_t i,j;

    uint16_t region_width = x_end - x_start;
    uint16_t region_height = y_end - y_start;

    wavesharestm_send_command(0x61);//Set Resolution setting
    wavesharestm_send_data(0x02);
    wavesharestm_send_data(0x58);
    wavesharestm_send_data(0x01);
    wavesharestm_send_data(0xC0);
    wavesharestm_send_command(0x10);

    for(i=0; i<height; i++) {
        for(j=0; j< width/2; j++) {
            if(i<region_height+y_start && i>=y_start && j<(region_width+x_start)/2 && j>=x_start/2) {
                wavesharestm_send_data(buffer[(j-x_start/2) + (region_width/2*(i-y_start))]);
            }
            else {
                wavesharestm_send_data(0x11);
            }
        }
    }
    wavesharestm_send_command(0x04);//0x04
    wavesharestm_waitbusy_high();
    wavesharestm_send_command(0x12);//0x12
    wavesharestm_waitbusy_high();
    wavesharestm_send_command(0x02);  //0x02
    wavesharestm_waitbusy_low();
    wavesharestm_delay(200);
}
