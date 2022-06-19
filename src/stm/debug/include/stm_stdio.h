//
// Created by Aidan Morgan on 9/6/2022.
//

#ifndef MORSDLE_STM_STDIO_H
#define MORSDLE_STM_STDIO_H

#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <sys/unistd.h>
#include <sys/errno.h>
#include "main.h"

typedef struct {
    UART_HandleTypeDef* uart;
} console_t;

void console_init(console_t* config);

#endif //MORSDLE_STM_STDIO_H
