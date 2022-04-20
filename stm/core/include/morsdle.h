//
// Created by aidan on 20/04/2022.
//

#ifndef __MORSDLE_H__
#define __MORSDLE_H__

#include "display.h"
#include "drawing.h"

typedef enum {
    MORSDLE_OK,
    MORSDLE_ERR_TOOMANYCOMMANDS
} morsdle_err_t;

#define MAX_DRAWING_COMMANDS 255
static drawing_command_t* drawing_commands = drawing_command_t [MAX_DRAWING_COMMANDS];

morsdle_err_t push_command(drawing_command_t command);

#endif //__MORSDLE_H__
