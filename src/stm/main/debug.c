//
// Created by Aidan Morgan on 9/6/2022.
//

#include "debug.h"
#include "cbuff.h"

typedef enum {
    WAITING,
    TRANSMITTING
} uart_state_t;

static UART_HandleTypeDef* huart;

static uint8_t printf_array [16];
static cbuff_t* printf_buffer = &(cbuff_t) {
};


static uart_state_t state = WAITING;

void debug_init(stm32_config_t* config) {
    huart = config->debug_uart_handle;

    cbuff_init(printf_buffer, &printf_array, 16, sizeof(uint8_t));

    setvbuf(stdout, NULL, _IONBF, 0);
}

static void debug_printf_transmit() {
    size_t count = cbuff_size(printf_buffer);

    if(count > 0) {
        state = TRANSMITTING;

        uint8_t data[count];
        cbuff_readmany(printf_buffer, &data, count);
        HAL_UART_Transmit_DMA(huart, &data, count);
    }
}

static void debug_printf_putc(uint8_t c){
    cbuff_write(printf_buffer, &c);

    if(state == WAITING) {
        debug_printf_transmit();
    }
}


int __io_putchar(int ch)
{
    debug_printf_putc(ch);
    return ch;
}


void HAL_UARTEx_WakeupCallback(UART_HandleTypeDef *h) {

}

// called by the underlying HAL when a transmit request has completed
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *h){
    if(h == huart) {
        debug_printf_transmit();
    }
}



