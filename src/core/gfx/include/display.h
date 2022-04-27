//
// Created by aidan on 21/04/2022.
//

#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>

typedef struct {
    uint16_t x;
    uint16_t y;
}point;
typedef point point_t;

typedef enum {
    COLOUR_BLACK,
    COLOUR_WHITE,
    COLOUR_GREEN,
    COLOUR_BLUE,
    COLOUR_RED,
    COLOUR_YELLOW,
    COLOUR_ORANGE
} colour_t;

// an opaque handle, defined by the implementor
typedef struct display_handle_t display_handle_t;

typedef struct {
    void (*draw_line)(display_handle_t* surface, point_t start, point_t end, uint8_t thickness, colour_t colour);
    void (*fill_rect)(display_handle_t* surface, point_t topleft, point_t bottomright, colour_t fill_colour);
    void (*draw_char)(display_handle_t* surface, char c, point_t topleft, uint8_t size, colour_t colour);

    display_handle_t* handle;

    uint16_t height;
    uint16_t width;
} display_operations;
typedef display_operations* display_operations_t;

extern void init_drawing_ops(display_operations_t ops);
extern void destroy_drawing_ops(display_operations_t ops);

#endif //DISPLAY_H
