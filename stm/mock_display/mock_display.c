//
// Created by aidan on 20/04/2022.
//
#include "display.h"

#define MOCK_DISPLAY_WIDTH 320
#define MOCK_DISPLAY_HEIGHT 420

display_err_t display_surface_init(display_surface_t* surface) {
    surface->height = MOCK_DISPLAY_HEIGHT;
    surface->width = MOCK_DISPLAY_WIDTH;

}

display_err_t display_surface_render(display_surface_t* surface) {

}

display_err_t display_surface_destroy(display_surface_t* surface) {

}