//
// Created by aidan on 22/04/2022.
//

#ifndef SVG_H
#define SVG_H
#include<stdlib.h>
#include<stdbool.h>
#include<stdio.h>
#include <stdint.h>
#include<math.h>

// --------------------------------------------------------
// STRUCT svg
// --------------------------------------------------------
typedef struct svg
{
    char* svg;
    uint32_t height;
    uint32_t width;
    bool finalized;
} svg;

svg* svg_create(int32_t width, int32_t height);
void svg_finalize(svg* psvg);
void svg_print(svg* psvg);
void svg_save(svg* psvg, const char* filepath);
void svg_free(svg* psvg);

void svg_circle(svg* psvg, const char* stroke, int32_t strokewidth, const char* fill, int32_t r, int32_t cx, int32_t cy);
void svg_line(svg* psvg, const char* stroke, int32_t strokewidth, int32_t x1, int32_t y1, int32_t x2, int32_t y2);
void svg_rectangle(svg* psvg, int32_t width, int32_t height, int32_t x, int32_t y, const char* fill, const char* stroke, int32_t strokewidth, int32_t radiusx, int32_t radiusy);
void svg_fill(svg* psvg, const char* fill);
void svg_text(svg* psvg, int32_t x, int32_t y, const char* fontfamily, int32_t fontsize, const char* fill, const char* stroke, const char* text);
void svg_ellipse(svg* psvg, int32_t cx, int32_t cy, int32_t rx, int32_t ry, const char* fill, const char* stroke, int32_t strokewidth);


#endif //SVG_H
