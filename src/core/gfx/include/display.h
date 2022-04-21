//
// Created by aidan on 20/04/2022.
//

#ifndef __DISPLAY_H__
#define __DISPLAY_H__
#include <stdint.h>

struct display_surface_instance_t {
    void * surface_data;
    const uint8_t width;
    const uint8_t height;
};

typedef struct display_surface_instance_t* display_surface_t;

typedef enum {
    DISPLAY_OK,
    DISPLAY_ERR_NOTIMPLEMENTED
} display_err_t;

typedef enum {
    BLACK,
    WHITE,
    GREEN,
    BLUE,
    RED,
    YELLOW,
    ORANGE
} surface_colour_t;

typedef struct {
    surface_colour_t foreground;
    surface_colour_t background;
} surface_options_t;

extern display_err_t display_surface_init(display_surface_t surface);
extern display_err_t display_surface_render(display_surface_t surface);
extern void display_surface_set(display_surface_t, uint8_t x, uint8_t y, surface_options_t opts);
extern void display_surface_clear(display_surface_t, uint8_t x, uint8_t y);
extern void display_surface_fill(display_surface_t, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, surface_options_t opts);
extern display_err_t display_surface_destroy(display_surface_t surface);


#endif //__DISPLAY_H__
