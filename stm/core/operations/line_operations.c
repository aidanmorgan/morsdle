#include "operations/line_operations.h"

struct line_command_data_t {
    struct drawing_point_t start;
    struct drawing_point_t end;
    float opacity;
};

drawing_operation_error_t line_draw(display_surface_t surface, struct drawing_rectangle_t bbox, void*operation_data, void* cd) {
    struct line_command_data_t* command_data = (struct line_command_data_t*)cd;

    for(uint8_t y = bbox.top_left.y; y < bbox.bottom_right.y; y++) {
        for(uint8_t x = bbox.top_left.x; x < bbox.bottom_right.x; x++) {
            uint8_t x_dash = (uint8_t)round((y-bbox.top_left.y)*(bbox.bottom_right.x-bbox.top_left.x)/(bbox.bottom_right.y-bbox.top_left.y) + x);
            display_surface_set(surface, x_dash, y, command_data->opacity);
        }
    }

    return DRAWING_OPERATION_OK;
}

void line_bbox(struct drawing_rectangle_t* bbox, void* operation_data, void* command_data) {
    struct line_command_data_t* data = (struct line_command_data_t*)command_data;
    bbox->top_left = data->start;
    bbox->bottom_right = data->end;
}

drawing_command_t line_create(struct drawing_point_t* top_left, struct drawing_point_t* bottom_right, float opacity) {
    drawing_command_t command = INLINE_MALLOC(struct drawing_command_instance_t,
            .top_left = top_left,
            .name = "line",
            .command_data = INLINE_MALLOC(struct line_command_data_t
                .start = INLINE_MALLOC(drawing_point_t,
                        .x = top_left->x,
                        .y = top_left->y
                ),
                .end = INLINE_MALLOC(drawing_point_t,
                        .x = bottom_right->x,
                        .y = bottom_right->y
                ),
                .opacity = opacity,
            )
        );

    return (drawing_command_t)command;
}


REGISTER_DRAWING_OPERATION("line", line_draw, line_bbox)