#include "include/drawing.h"
#include <string.h>

void drawing_init()
{
    drawing_registered_operations = llist_create();
}

void drawing_register_operation(char * const name,
                                drawing_operation_error_t (*draw)(display_surface_t, drawing_rectangle_t, void*,void*),
                                void (*bbox)(drawing_rectangle_t * bbox, void*, void*),
                                void*opData) {

    drawing_operation* op = (drawing_operation*)malloc(sizeof(drawing_operation));

    strcpy(op->operation_name, name);
    op->draw = draw;
    op->bbox = bbox;
    op->operation_data = opData;

    llist_insert(drawing_registered_operations, op);
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

static bool _find_drawing_operation_callback(void*context, void*item) {
    char* name = (char*)context;
    drawing_operation_t op = (drawing_operation_t)item;

    return strcmp(name, op->operation_name) == 0;
}

drawing_error_t drawing_draw_one(display_surface_t surface, drawing_command_t command) {
    llist_find_result_t result = llist_find(drawing_registered_operations, _find_drawing_operation_callback, command->operation_name);

    if(result->count == 0) {
        llist_dispose_find_result(result);
        return DRAWING_ERR_NOSUCHCOMMAND;
    }
    else if(result->count > 1) {
        llist_dispose_find_result(result);
        return DRAWING_ERR_MULTIPLECOMMANDS;
    }
    else {
        drawing_operation_t operation = (drawing_operation_t)result->items[0];
        // clean up the find result now we have the 1 item we actually want.
        llist_dispose_find_result(result);

        drawing_point_t top_left = command->top_left;

        drawing_rectangle_t operation_bbox;
        // request the operation to calculate the bounding-box that we're going to need to draw into
        // so we can check if we're going to draw off the extents of the display, stack based so we don't need to worry
        // about freeing this memory.
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
