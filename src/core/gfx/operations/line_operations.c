#include "operations/line_operations.h"

typedef struct {
    drawing_point_t start;
    drawing_point_t end;
    surface_options_t options;
} line_command_data_t;

void line_bbox_impl(drawing_rectangle_t* bbox, void* operation_data, void* command_data);
drawing_operation_error_t line_draw_impl(display_surface_t surface, drawing_rectangle_t bbox, void* operation_data, void* command_data);

drawing_operation_error_t line_draw_impl(display_surface_t surface, drawing_rectangle_t bbox, void* operation_data, void* command_data) {
    line_command_data_t* line_data = (line_command_data_t*)command_data;

    for(uint8_t y = bbox.top_left.y; y < bbox.bottom_right.y; y++) {
        for(uint8_t x = bbox.top_left.x; x < bbox.bottom_right.x; x++) {
            uint8_t x_dash = (uint8_t)round((y-bbox.top_left.y)*(bbox.bottom_right.x-bbox.top_left.x)/(bbox.bottom_right.y-bbox.top_left.y) + x);
            display_surface_set(surface, x_dash, y, line_data->options);
        }
    }

    return DRAWING_OPERATION_OK;
}


void line_bbox_impl(drawing_rectangle_t* bbox, void* operation_data, void* command_data) {
    line_command_data_t* data = (line_command_data_t*)command_data;

    bbox->top_left.x = data->start.x;
    bbox->top_left.y = data->start.y;

    bbox->bottom_right.x = data->end.x;
    bbox->bottom_right.y = data->end.y;
}

drawing_command_t line_create(drawing_point_t* top_left, drawing_point_t* bottom_right, surface_options_t opts) {
    drawing_point_t* start = (drawing_point_t*)INLINE_MALLOC(drawing_point_t,
                                                           .x = top_left->x,
                                                           .y = top_left->y
    );

    drawing_point_t* end = (drawing_point_t*)INLINE_MALLOC(drawing_point_t,
                                   .x = bottom_right->x,
                                   .y = bottom_right->y
    );

    drawing_command_t command = (drawing_command_t)INLINE_MALLOC(drawing_command,
            .top_left = *top_left,
            .operation_name = "line",
            .command_data = (line_command_data_t*)INLINE_MALLOC(line_command_data_t,
                                                                .end = *end,
                                                                .start = *start,
                                                                .options = opts)

        );



    return command;
}

void line_destroy(drawing_command_t cmd) {

}

/*
drawing_register_operation("line", line_draw_impl, line_bbox_impl, NULL);
*/