#ifndef __DRAWING_H__
#define __DRAWING_H__

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "allocations.h"
#include "display.h"

// this gives a total square area of 96x64 pixels, which is actually 94x62 when we include a line and a 1px spacer

typedef enum  {
    DRAWING_OK,
    DRAWING_ERR_NOSUCHCOMMAND,
    DRAWING_ERR_BBOXOUTOFRANGE,
    DRAWING_ERR_OPERATIONFAILED,
    DRAWING_ERR_DISPLAYERROR
} drawing_error_t;

typedef enum {
    DRAWING_OPERATION_OK,
    DRAWING_OPERATION_ERR_FAILED
} drawing_operation_error_t;


typedef struct {
    uint8_t x;
    uint8_t y;
} drawing_point_t;

typedef struct {
    drawing_point_t top_left;
    drawing_point_t bottom_right;
} drawing_rectangle_t;

// convenience macros for calculating height and width of a rectangle
#define DRAWING_RECTANGLE_WIDTH(p) (uint8_t)(p.bottom_right.x - p.top_left.x)
#define DRAWING_RECTANGLE_HEIGHT(p) (uint8_t)(p.bottom_right.y - p.top_left.y)

#define MAX_DRAWING_OPERATIONS 255

// a drawing command is an instance of a drawing operation that is to be performed using the provided parameters
typedef struct  {
    char* name;
    drawing_point_t top_left;
    // provides the drawing specific implementation information for the renderer
    void* command_data;
} drawing_command;
typedef drawing_command* drawing_command_t;


// a drawing operation is a struct that defines the drawing operations that can be performed
typedef struct {
    char* name;

    // function pointer to the function that should be used to draw instances
    drawing_operation_error_t (*draw)(display_surface_t, drawing_rectangle_t, void*,void*);
    // function pointer to the function that calculates the bounding box for the operation
    void (*bbox)(drawing_rectangle_t * bbox, void*, void*);

    // allows data that is shared between all drawing commands with the same name to be used
    void* operation_data;
} drawing_operation;
typedef drawing_operation* drawing_operation_t;


static size_t drawing_operation_count = 0;
static drawing_operation DRAWING_OPERATIONS[MAX_DRAWING_OPERATIONS];

#define REGISTER_DRAWING_OPERATION(x,y,z ) drawing_register_operation((drawing_operation_count++), x, y, z, NULL);
#define REGISTER_DRAWING_OPERATION_WITH_DATA(w, x,y,z) drawing_register_operation((drawing_operation_count++), w, x, y, z);

void drawing_register_operation(char * const name,
                                drawing_operation_error_t (*draw)(display_surface_t, drawing_rectangle_t, void*,void*),
                                void (*bbox)(drawing_rectangle_t * bbox, void*, void*),
                                void*opData) {
    uint8_t index = drawing_operation_count+1;

    strcpy(DRAWING_OPERATIONS[index].name, name);
    DRAWING_OPERATIONS[index].draw = draw;
    DRAWING_OPERATIONS[index].bbox = bbox;
    DRAWING_OPERATIONS[index].operation_data = opData;
}

drawing_error_t drawing_draw_one(display_surface_t surface, drawing_command_t command);
drawing_error_t drawing_draw(display_surface_t surface, drawing_command_t* commands, size_t len);

double_t drawing_point_distance(drawing_point_t start, drawing_point_t end);

#endif