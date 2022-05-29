//
// Created by Aidan Morgan on 27/5/2022.
//

#include "waveshare_stm.h"

static bool _get_pin_details(wavesharestm_conf_t conf, wavesharestm_pin_t pintype, GPIO_TypeDef *port, uint16_t *pin) {
    if (pintype == RST) {
        *port = *conf->rst.port;
        *pin = conf->rst.pin;

        return true;
    } else if (pintype == CS) {
        *port = *conf->cs.port;
        *pin = conf->cs.pin;

        return true;
    } else if (pintype == DC) {
        *port = *conf->dc.port;
        *pin = conf->dc.pin;

        return true;
    } else if (pintype == BUSY) {
        *port = *conf->busy.port;
        *pin = conf->busy.pin;

        return true;
    }

    return false;
}

static void inline wavesharestm_digital_write(wavesharestm_conf_t conf, wavesharestm_pin_t request, uint8_t val) {
    GPIO_TypeDef port;
    uint16_t pin = 0;

    if (!_get_pin_details(conf, request, &port, &pin)) {
        return;
    }

    HAL_GPIO_WritePin(&port, pin, val == 0 ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

static void inline wavesharestm_digital_read(wavesharestm_conf_t conf, wavesharestm_pin_t request, uint8_t *result) {
    GPIO_TypeDef port;
    uint16_t pin;

    if (!_get_pin_details(conf, request, &port, &pin)) {
        return;
    }

    *result = HAL_GPIO_ReadPin(&port, pin);
}

static void inline wavesharestm_spi_write(wavesharestm_conf_t conf, uint8_t data) {

}

static void inline wavesharestm_spi_read(wavesharestm_conf_t conf, uint8_t *data) {
    // no op
}

static void inline wavesharestm_waitbusy_high(wavesharestm_conf_t conf) {
    uint8_t data = 1;
    while (!data) {
        wavesharestm_digital_read(conf, BUSY, &data);
    }
}

static void inline wavesharestm_waitbusy_low(wavesharestm_conf_t conf) {
    uint8_t data = 0;
    while (data) {
        wavesharestm_digital_read(conf, BUSY, &data);
    }
}


void wavesharestm_send_data(wavesharestm_conf_t conf, uint8_t data) {
    wavesharestm_digital_write(conf, DC, 1);
    wavesharestm_digital_write(conf, CS, 0);
    wavesharestm_spi_write(conf, data);
    wavesharestm_digital_write(conf, CS, 1);
}

void wavesharestm_send_command(wavesharestm_conf_t conf, uint8_t data) {
    wavesharestm_digital_write(conf, DC, 0);
    wavesharestm_digital_write(conf, DC, 0);
    wavesharestm_spi_write(conf, data);
    wavesharestm_digital_write(conf, DC, 1);
}

void wavesharestm_delay(wavesharestm_conf_t conf, uint32_t ms) {
    HAL_Delay(ms);
}

void wavesharestm_reset(wavesharestm_conf_t conf) {
    wavesharestm_digital_write(conf, RST, 1);
    wavesharestm_delay(conf, 200);
    wavesharestm_digital_write(conf, ST, 0);
    wavesharestm_delay(conf, 1);
    wavesharestm_digital_write(conf, RST, 1);
    wavesharestm_delay(conf, 200);
}

void wavesharestm_clear(wavesharestm_conf_t conf, uint16_t width, uint16_t height, uint8_t color) {
    wavesharestm_send_command(conf, 0x61);//Set Resolution setting
    wavesharestm_send_data(conf, 0x02);
    wavesharestm_send_data(conf, 0x58);
    wavesharestm_send_data(conf, 0x01);
    wavesharestm_send_data(conf, 0xC0);
    wavesharestm_send_command(conf, 0x10);

    for (int i = 0; i < width / 2; i++) {
        for (int j = 0; j < height; j++)
            wavesharestm_send_data(conf, (color << 4) | color);
    }
    wavesharestm_send_command(conf, 0x04);//0x04
    wavesharestm_waitbusy_high(conf);
    wavesharestm_send_command(conf, 0x12);//0x12
    wavesharestm_waitbusy_high(conf);
    wavesharestm_send_command(conf, 0x02);  //0x02
    wavesharestm_waitbusy_low(conf);
    wavesharestm_delay(conf, 500);
}


void wavesharestm_init(wavesharestm_conf_t conf) {
    wavesharestm_reset(conf);

    wavesharestm_waitbusy_high(conf);
    wavesharestm_send_command(conf, 0x00);
    wavesharestm_send_data(conf, 0xEF);
    wavesharestm_send_data(conf, 0x08);
    wavesharestm_send_command(conf, 0x01);
    wavesharestm_send_data(conf, 0x37);
    wavesharestm_send_data(conf, 0x00);
    wavesharestm_send_data(conf, 0x23);
    wavesharestm_send_data(conf, 0x23);
    wavesharestm_send_command(conf, 0x03);
    wavesharestm_send_data(conf, 0x00);
    wavesharestm_send_command(conf, 0x06);
    wavesharestm_send_data(conf, 0xC7);
    wavesharestm_send_data(conf, 0xC7);
    wavesharestm_send_data(conf, 0x1D);
    wavesharestm_send_command(conf, 0x30);
    wavesharestm_send_data(conf, 0x3C);
    wavesharestm_send_command(conf, 0x41);
    wavesharestm_send_data(conf, 0x00);
    wavesharestm_send_command(conf, 0x50);
    wavesharestm_send_data(conf, 0x37);
    wavesharestm_send_command(conf, 0x60);
    wavesharestm_send_data(conf, 0x22);
    wavesharestm_send_command(conf, 0x61);
    wavesharestm_send_data(conf, 0x02);
    wavesharestm_send_data(conf, 0x58);
    wavesharestm_send_data(conf, 0x01);
    wavesharestm_send_data(conf, 0xC0);
    wavesharestm_send_command(conf, 0xE3);
    wavesharestm_send_data(conf, 0xAA);

    wavesharestm_delay(conf, 100);
    wavesharestm_send_command(conf, 0x50);
    wavesharestm_send_data(conf, 0x37);
}

void wavesharestm_destroy(wavesharestm_conf_t conf) {

}

void wavesharestm_render_impl(void *data, uint8_t *buffer, uint16_t x_start, uint16_t y_start, uint16_t x_end,
                              uint16_t y_end) {
    wavesharestm_conf_t conf = (wavesharestm_conf_t) data;

    uint64_t i, j;

    wavesharestm_send_command(conf, 0x61);
    wavesharestm_send_data(conf, 0x02);
    wavesharestm_send_data(conf, 0x58);
    wavesharestm_send_data(conf, 0x01);
    wavesharestm_send_data(conf, 0xC0);
    wavesharestm_send_command(conf, 0x10);

    for (i = 0; i < conf->height; i++) {
        for (j = 0; j < conf->width / 2; j++) {
            wavesharestm_send_data(conf, buffer[j + ((conf->width / 2) * i)]);
        }
    }
    wavesharestm_send_command(conf, 0x04);//0x04
    wavesharestm_waitbusy_high(conf);
    wavesharestm_send_command(conf, 0x12);//0x12
    wavesharestm_waitbusy_high(conf);
    wavesharestm_send_command(conf, 0x02);  //0x02
    wavesharestm_waitbusy_low(conf);
    wavesharestm_delay(conf, 200);
}
