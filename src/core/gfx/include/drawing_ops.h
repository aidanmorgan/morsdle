//
// Created by aidan on 21/04/2022.
//

#ifndef DRAWING_OPS_H
#define DRAWING_OPS_H

#include <stdint.h>

typedef struct {
    uint16_t x;
    uint16_t y;
}point;
typedef point point_t;

typedef struct {
    point_t top_left;
    point_t bottom_right;
} rectangle;
typedef rectangle rectangle_t;

typedef enum {
    COLOUR_WHITE,
    COLOUR_BLACK,
    COLOUR_BLUE,
    COLOUR_RED,
    COLOUR_ORANGE,
    COLOUR_GREEN
} colour_t;

typedef struct {
    void (*draw_line)(void* surface, point_t start, point_t end, uint8_t thickness, colour_t colour);
    void (*fill_rect)(void* surface, point_t topleft, point_t bottomright, colour_t fill_colour);
    void (*draw_text)(void* surface, char c, point_t topleft, uint8_t size, colour_t colour);

    // a handle that is injected by the relevant implementation of the underlying drawing provider
    // do not use, except as a parameter to the drawing functions above.
    void* handle;

    uint16_t height;
    uint16_t width;
} drawing_ops;
typedef drawing_ops* drawing_ops_t;

extern void init_drawing_ops(drawing_ops_t ops);
extern void destroy_drawing_ops(drawing_ops_t ops);

#endif //DRAWING_OPS_H
