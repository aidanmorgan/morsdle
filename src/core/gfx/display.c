#include "display.h"

void display_add_region(render_pass_t* pass, rectangle_t rect) {
    cbuff_write(pass->dirty_regions, &rect);
}

bool display_has_regions(render_pass_t* pass) {
    return cbuff_size(pass->dirty_regions) > 0;
}


