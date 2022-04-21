//
// Created by aidan on 21/04/2022.
//

#ifndef RECTANGLE_OPERATIONS_H
#define RECTANGLE_OPERATIONS_H

#include "../drawing.h"
#include "../display.h"

drawing_command_t rectangle_create(drawing_point_t* top_left, drawing_point_t* bottom_right, surface_options_t opts);
void rectangle_destroy(drawing_command_t cmd);


#endif //RECTANGLE_OPERATIONS_H
