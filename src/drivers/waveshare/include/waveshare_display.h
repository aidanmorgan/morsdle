//
// Created by aidan on 21/04/2022.
//

#ifndef WAVESHARE_DISPLAY_H
#define WAVESHARE_DISPLAY_H


#include "morsdle.h"
#include "display.h"
#include "renderer.h"

#define WAVESHARE_BLACK   0x0	/// 000
#define WAVESHARE_WHITE   0x1	///	001
#define WAVESHARE_GREEN   0x2	///	010
#define WAVESHARE_BLUE    0x3	///	011
#define WAVESHARE_RED     0x4	///	100
#define WAVESHARE_YELLOW  0x5	///	101
#define WAVESHARE_ORANGE  0x6	///	110

enum waveshare_display_state {
    WAVESHARE_DISPLAY_UNKNOWN,
    WAVESHARE_DISPLAY_INITIALISED,
    WAVESHARE_DISPLAY_AWAKE,
    WAVESHARE_DISPLAY_ALSEEP
};
typedef enum waveshare_display_state waveshare_display_state_t;

struct display_impl_t  {
    uint8_t* buffer;
    // this is the callback to perform the actual render, required to be set by the implementation
    void (*render_dirty_region)(uint8_t*, uint16_t, uint16_t, uint16_t, uint16_t, uint16_t, uint16_t);
    // callback to wake the display up if it's been put to sleep
    void (*init)();
    void (*wake)();

    // callback to put the display to sleep if it's been sleeping
    void (*sleep)();

    waveshare_display_state_t state;
};

void waveshare_draw_line(render_pass_t pass, point_t start, point_t end, uint8_t thickness, colour_t colour);
void waveshare_fill_rect(render_pass_t pass,point_t topleft, point_t bottomright, colour_t fill_colour);
void waveshare_draw_char(render_pass_t pass, char c, point_t topleft, uint8_t size, colour_t colour);


#endif //WAVESHARE_DISPLAY_H
