//
// Created by aidan on 20/04/2022.
//

#ifndef __DISPLAY_H__
#define __DISPLAY_H__

struct display_surface_instance_t {
    void * surface_data;
    const uint8_t width;
    const uint8_t height;
};

typedef struct display_surface_instance_t* display_surface_t;

typedef enum {
    DISPLAY_OK,
} display_err_t;

extern display_err_t display_surface_init(display_surface_t surface);
extern display_err_t display_surface_render(display_surface_t surface);
extern void display_surface_set(display_surface_t, uint8_t x, uint8_t y, float opacity);
extern void display_surface_clear(display_surface_t, uint8_t x, uint8_t y, float opacity);
extern display_err_t display_surface_destroy(display_surface_t surface);


#endif //__DISPLAY_H__
