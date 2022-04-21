//
// Created by aidan on 20/04/2022.
//

#ifndef LINE_OPERATIONS_H
#define LINE_OPERATIONS_H

#include "../drawing.h"
#include "../display.h"

drawing_command_t line_create(drawing_point_t* top_left, drawing_point_t* bottom_right, surface_options_t opts);
void line_destroy(drawing_command_t cmd);

#endif //LINE_OPERATIONS_H
