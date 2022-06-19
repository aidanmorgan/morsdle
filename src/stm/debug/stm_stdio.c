//
// Created by Aidan Morgan on 9/6/2022.
//

#include <malloc.h>
#include "stm_stdio.h"

static UART_HandleTypeDef* console_uart;

void console_init(console_t* config) {
    setvbuf(stdin, NULL, _IONBF, 0);
    console_uart = config->uart;
}

int fputc(int ch, FILE *f)
{
    HAL_UART_Transmit(console_uart, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
}

int fgetc(FILE *f) {
    uint8_t ch = 0;

    /* Clear the Overrun flag just before receiving the first character */
    __HAL_UART_CLEAR_OREFLAG(console_uart);

    /* Wait for reception of a character on the USART RX line */
    HAL_UART_Receive(console_uart, (uint8_t *)&ch, 1, HAL_MAX_DELAY);

    return ch;
}


