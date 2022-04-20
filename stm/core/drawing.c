#include "drawing.h"
#include <string.h>

drawing_operation_t drawing_find_operation(const char* name) {
    drawing_operation_t op;
    uint8_t counter = 0;

    while(op == NULL && counter < drawing_operation_count) {
        if(strcmp(DRAWING_OPERATIONS[counter].name, name) == 0) {
            op = (drawing_operation_t)&DRAWING_OPERATIONS[counter];
        }
        else {
            counter++;
        }
    }

    return op;
}

bool drawing_is_in_bounds(display_surface_t surface, drawing_rectangle_t bbox) {
    if(bbox.bottom_right.x < 0 || bbox.bottom_right.x > surface->width) {
        return false;
    }

    if(bbox.top_left.x < 0 || bbox.top_left.x > surface->width) {
        return false;
    }

    if(bbox.bottom_right.y < 0 || bbox.bottom_right.y > surface->height) {
        return false;
    }

    if(bbox.bottom_right.x < 0 || bbox.bottom_right.y > surface->height) {
        return false;
    }

    return true;
}

drawing_error_t drawing_draw_one(display_surface_t surface, drawing_command_t command) {
    drawing_operation_t operation = drawing_find_operation(command->name);

    if(operation == NULL) {
        return DRAWING_ERR_NOSUCHCOMMAND;
    }
    else {
        drawing_point_t top_left = command->top_left;

        drawing_rectangle_t operation_bbox;
        // request the operation to calculate the bounding-box that we're going to need to draw into
        // so we can check if we're going to draw off the extents of the display
        operation->bbox(&operation_bbox, operation->operation_data, command->command_data);

        // stack allocation is fine here, it's only needed for the scope of the call
        drawing_rectangle_t bbox = (drawing_rectangle_t) {
            .top_left = top_left,
            .bottom_right = (drawing_point_t) {
                .x = top_left.x + DRAWING_RECTANGLE_WIDTH(operation_bbox),
                .y = top_left.y + DRAWING_RECTANGLE_HEIGHT(operation_bbox)
            }
        };

        if(!drawing_is_in_bounds(surface, bbox)) {
            return DRAWING_ERR_BBOXOUTOFRANGE;
        }

        drawing_operation_error_t operation_result = operation->draw(surface, bbox, operation->operation_data, command->command_data);

        if(operation_result == DRAWING_OPERATION_ERR_FAILED) {
            return DRAWING_ERR_OPERATIONFAILED;
        }
        else {
            return DRAWING_OK;
        }
    }
}

drawing_error_t drawing_draw(display_surface_t surface, drawing_command_t* commands, size_t len) {
    uint8_t i = 0;
    for(i = 0; i < len; i++) {
        drawing_command_t command = commands[i];
        drawing_error_t err = drawing_draw_one(surface, command);

        if(err != DRAWING_OPERATION_OK) {
            return DRAWING_ERR_OPERATIONFAILED;
        }
    }

    display_err_t err = display_surface_render(surface);
    if(err != DISPLAY_OK) {
        return DRAWING_ERR_DISPLAYERROR;
    }

    err = display_surface_destroy(surface);
    if(err != DISPLAY_OK) {
        return DRAWING_ERR_DISPLAYERROR;
    }

    return DRAWING_OK;
}

double_t drawing_point_distance(drawing_point_t start, drawing_point_t end) {
    return sqrt( (end.x - start.x)*(end.x- start.x) + (end.y - start.y)*(end.y - start.y) );
}
