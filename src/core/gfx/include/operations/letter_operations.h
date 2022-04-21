//
// Created by aidan on 20/04/2022.
//

#ifndef LETTER_OPERATIONS_H
#define LETTER_OPERATIONS_H
#include "../drawing.h"
#include "../display.h"

drawing_command_t letter_create(char letter, surface_options_t opts);
void letter_destroy(drawing_command_t cmd);


#endif //LETTER_OPERATIONS_H
