#include "canvas.h"


size_t renderpass_read_regions(render_pass_t* pass, rectangle_t* out, size_t size) {
    return cbuff_readmany(pass->dirty_regions, out, size);
}

bool renderpass_clear_regions(render_pass_t* pass) {
    return cbuff_clear(pass->dirty_regions);
}

bool renderpass_add_region(render_pass_t* pass, point_t ul, point_t br) {
    rectangle_t rect = (rectangle_t) {
        .top_left = ul,
        .bottom_right = br
    };

    return cbuff_write(pass->dirty_regions, &rect);
}

size_t renderpass_regions_size(render_pass_t* pass) {
    return cbuff_size(pass->dirty_regions);
}


