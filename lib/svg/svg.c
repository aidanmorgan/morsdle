
#include<stdlib.h>
#include<stdbool.h>
#include<stdio.h>
#include<string.h>
#include<math.h>

#include"svg.h"

static void appendstringtosvg(svg* psvg, const char* text) {
    char* p = realloc(psvg->svg, strlen(psvg->svg) + strlen(text) + 1);

    if(p) {
        psvg->svg = p;
    }

    strcat(psvg->svg, text);
}

static void appendnumbertosvg(svg* psvg, int32_t n) {
    char sn[16];
    sprintf(sn, "%d", n);

    appendstringtosvg(psvg, sn);
}

svg* svg_create(int32_t width, int32_t height) {
    svg* psvg = malloc(sizeof(svg));

    if(psvg != NULL) {
        *psvg = (svg){.svg = NULL, .width = width, .height = height, .finalized = false};
        psvg->svg = malloc(1);

        sprintf(psvg->svg, "%s", "\0");
        appendstringtosvg(psvg, "<svg width='");
        appendnumbertosvg(psvg, width);
        appendstringtosvg(psvg, "px' height='");
        appendnumbertosvg(psvg, height);
        appendstringtosvg(psvg, "px' xmlns='http://www.w3.org/2000/svg' version='1.1' xmlns:xlink='http://www.w3.org/1999/xlink'>\n");

        return psvg;
    }
    else
    {
        return NULL;
    }
}

void svg_finalize(svg* psvg) {
    appendstringtosvg(psvg, "</svg>");
    psvg->finalized = true;
}

void svg_print(svg* psvg) {
    printf("%s\n", psvg->svg);
}

void svg_save(svg* psvg, const char* filepath) {
    if(!psvg->finalized) {
        svg_finalize(psvg);
    }

    FILE* fp = fopen(filepath, "w");

    if(fp != NULL) {
        fwrite(psvg->svg, 1, strlen(psvg->svg), fp);
        fclose(fp);
    }
}

void svg_free(svg* psvg) {
    free(psvg->svg);
    free(psvg);
}

void svg_circle(svg* psvg, const char* stroke, int32_t strokewidth, const char* fill, int32_t r, int32_t cx, int32_t cy) {
    appendstringtosvg(psvg, "    <circle stroke='");
    appendstringtosvg(psvg, stroke);
    appendstringtosvg(psvg, "' stroke-width='");
    appendnumbertosvg(psvg, strokewidth);
    appendstringtosvg(psvg, "px' fill='");
    appendstringtosvg(psvg, fill);
    appendstringtosvg(psvg, "' r='");
    appendnumbertosvg(psvg, r);
    appendstringtosvg(psvg, "' cy='");
    appendnumbertosvg(psvg, cy);
    appendstringtosvg(psvg, "' cx='");
    appendnumbertosvg(psvg, cx);
    appendstringtosvg(psvg, "' />\n");
}

void svg_line(svg* psvg, const char* stroke, int32_t strokewidth, int32_t x1, int32_t y1, int32_t x2, int32_t y2) {
    appendstringtosvg(psvg, "    <line stroke='");
    appendstringtosvg(psvg, stroke);
    appendstringtosvg(psvg, "' stroke-width='");
    appendnumbertosvg(psvg, strokewidth);
    appendstringtosvg(psvg, "px' y2='");
    appendnumbertosvg(psvg, y2);
    appendstringtosvg(psvg, "' x2='");
    appendnumbertosvg(psvg, x2);
    appendstringtosvg(psvg, "' y1='");
    appendnumbertosvg(psvg, y1);
    appendstringtosvg(psvg, "' x1='");
    appendnumbertosvg(psvg, x1);
    appendstringtosvg(psvg, "' />\n");
}

void svg_rectangle(svg* psvg, int32_t width, int32_t height, int32_t x, int32_t y, const char* fill, const char* stroke, int32_t strokewidth, int32_t radiusx, int32_t radiusy) {
    appendstringtosvg(psvg, "    <rect ");

    if(fill != NULL) {
        appendstringtosvg(psvg, "fill='");
        appendstringtosvg(psvg, fill);
        appendstringtosvg(psvg, "' ");
    }

    if(stroke != NULL) {
        appendstringtosvg(psvg, "stroke='");
        appendstringtosvg(psvg, stroke);
        appendstringtosvg(psvg, "' stroke-width='");
        appendnumbertosvg(psvg, strokewidth);
        appendstringtosvg(psvg, "px' ");
    }
    appendstringtosvg(psvg, "width='");
    appendnumbertosvg(psvg, width);
    appendstringtosvg(psvg, "' height='");
    appendnumbertosvg(psvg, height);
    appendstringtosvg(psvg, "' y='");
    appendnumbertosvg(psvg, y);
    appendstringtosvg(psvg, "' x='");
    appendnumbertosvg(psvg, x);
    appendstringtosvg(psvg, "'");

    if(radiusy > 0) {
        appendstringtosvg(psvg, "ry='");
        appendnumbertosvg(psvg, radiusy);
        appendstringtosvg(psvg, "' ");
    }

    if(radiusx > 0) {
        appendstringtosvg(psvg, "' rx='");
        appendnumbertosvg(psvg, radiusx);
        appendstringtosvg(psvg, "' ");
    }
    appendstringtosvg(psvg, "/>\n");
}

void svg_fill(svg* psvg, const char* fill) {
    svg_rectangle(psvg, psvg->width, psvg->height, 0, 0, fill, fill, 0, 0, 0);
}

void svg_text(svg* psvg, int32_t x, int32_t y, const char* fontfamily, int32_t fontsize, const char* fill, const char* stroke, const char* text) {
    appendstringtosvg(psvg, "    <text x='");
    appendnumbertosvg(psvg, x);
    appendstringtosvg(psvg, "' y = '");
    appendnumbertosvg(psvg, y);
    appendstringtosvg(psvg, "' font-family='");
    appendstringtosvg(psvg, fontfamily);
    appendstringtosvg(psvg, "' stroke='");
    appendstringtosvg(psvg, stroke);
    appendstringtosvg(psvg, "' fill='");
    appendstringtosvg(psvg, fill);
    appendstringtosvg(psvg, "' font-size='");
    appendnumbertosvg(psvg, fontsize);
    appendstringtosvg(psvg, "px'>");
    appendstringtosvg(psvg, text);
    appendstringtosvg(psvg, "</text>\n");
}

void svg_ellipse(svg* psvg, int32_t cx, int32_t cy, int32_t rx, int32_t ry, const char* fill, const char* stroke, int32_t strokewidth) {
    appendstringtosvg(psvg, "    <ellipse cx='");
    appendnumbertosvg(psvg, cx);
    appendstringtosvg(psvg, "' cy='");
    appendnumbertosvg(psvg, cy);
    appendstringtosvg(psvg, "' rx='");
    appendnumbertosvg(psvg, rx);
    appendstringtosvg(psvg, "' ry='");
    appendnumbertosvg(psvg, ry);
    appendstringtosvg(psvg, "' fill='");
    appendstringtosvg(psvg, fill);
    appendstringtosvg(psvg, "' stroke='");
    appendstringtosvg(psvg, stroke);
    appendstringtosvg(psvg, "' stroke-width='");
    appendnumbertosvg(psvg, strokewidth);
    appendstringtosvg(psvg, "' />\n");
}
