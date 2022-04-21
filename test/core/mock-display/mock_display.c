//
// Created by aidan on 20/04/2022.
//
#include "../../../src/core/gfx/include/display.h"

#define MOCK_DISPLAY_WIDTH 320
#define MOCK_DISPLAY_HEIGHT 420

display_err_t display_surface_init(display_surface_t surface) {
    return DISPLAY_ERR_NOTIMPLEMENTED;
}
display_err_t display_surface_render(display_surface_t surface) {
    return DISPLAY_ERR_NOTIMPLEMENTED;
}
void display_surface_set(display_surface_t surface, uint8_t x, uint8_t y, surface_options_t opts) {
    // no op
}
void display_surface_clear(display_surface_t surface, uint8_t x, uint8_t y) {
    // no op
}

void display_surface_fill(display_surface_t surface, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, surface_options_t opts) {
    // no op
}
display_err_t display_surface_destroy(display_surface_t surface) {
    return DISPLAY_ERR_NOTIMPLEMENTED;
}