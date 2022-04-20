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


struct drawing_point_t{
    uint8_t x;
    uint8_t y;
};

struct drawing_rectangle_t {
    struct drawing_point_t top_left;
    struct drawing_point_t bottom_right;
};

// convenience macros for calculating height and width of a rectangle
#define DRAWING_RECTANGLE_WIDTH(p) (uint8_t)(p.bottom_right.x - p.top_left.x)
#define DRAWING_RECTANGLE_HEIGHT(p) (uint8_t)(p.bottom_right.y - p.top_left.y)

#define MAX_DRAWING_OPERATIONS 255
#define REGISTER_DRAWING_OPERATION(x,y,z ) drawing_operations[drawing_operation_count++] = (drawing_operation_t)&(struct drawing_operation_instance_t){.name=x, .draw=y, .bbox=z, .data = NULL};
#define REGISTER_DRAWING_OPERATION_WITH_DATA(w, x,y,z) drawing_operations[drawing_operation_count++] = (drawing_operation_t)&(struct drawing_operation_instance_t){.name=w, .draw=x, .bbox=y, .data = z};


// a drawing operation is a struct that defines the drawing operations that can be performed
struct drawing_operation_instance_t{
    char* name;

    // function pointer to the function that should be used to draw instances
    drawing_operation_error_t (*draw)(display_surface_t, struct drawing_rectangle_t, void*,void*);
    // function pointer to the function that calculates the bounding box for the operation
    void (*bbox)(struct drawing_rectangle_t * bbox, void*, void*);

    // allows data that is shared between all drawing commands with the same name to be used
    void* operation_data;
};
typedef struct drawing_operation_instance_t* drawing_operation_t;

// a drawing command is an instance of a drawing operation that is to be performed using the provided parameters
struct drawing_command_instance_t{
    char* name;
    struct drawing_point_t top_left;
    // provides the drawing specific implementation information for the renderer
    void* command_data;
} ;
typedef struct drawing_command_instance_t* drawing_command_t;


size_t drawing_operation_count = 0;
drawing_operation_t drawing_operations[MAX_DRAWING_OPERATIONS];
drawing_operations[0] = (drawing_operation_t)&(struct drawing_operation_instance_t) {.name = "poop"};
drawing_operations[1] = (drawing_operation_t)&(struct drawing_operation_instance_t) {.name = "poop"};
drawing_operations[2] = (drawing_operation_t)&(struct drawing_operation_instance_t) {.name = "poop"};


drawing_error_t drawing_draw_one(display_surface_t surface, drawing_command_t command);
drawing_error_t drawing_draw(drawing_command_t* commands, size_t len);

double_t drawing_point_distance(struct drawing_point_t start, struct drawing_point_t end);

#endif