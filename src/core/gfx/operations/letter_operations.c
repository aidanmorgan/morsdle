#include "../include/drawing.h"
#include "operations/letter_operations.h"


#define LETTER_WIDTH_PX 50U
#define LETTER_HEIGHT_PX 50U

typedef struct {
    char letter;
    char* bitmap;
} letter_operation_data_t;

typedef struct {
    char letter;
    surface_options_t options;
} letter_command_data_t;

drawing_operation_error_t letter_draw(display_surface_t surface, drawing_rectangle_t bbox, void*operation_data, void*command_data) {
    letter_operation_data_t * data = (letter_operation_data_t*)operation_data;
    // we know what letter we have, but how the fuck do we render it

    // do drawing process
    return DRAWING_OPERATION_OK;
}

void letter_bbox(drawing_rectangle_t* bbox, void* operation_data, void*command_data) {
    bbox->top_left = *(drawing_point_t*)INLINE_MALLOC(drawing_point_t, .x = 0, .y = 0);
    bbox->bottom_right = *(drawing_point_t*)INLINE_MALLOC(drawing_point_t, .x = LETTER_WIDTH_PX, .y = LETTER_HEIGHT_PX);
}

drawing_command_t letter_create(char letter, surface_options_t opts) {
    // dodgy, I know, but we know that the name is fixed as there are only 26 letters in the alphabet
    char command_name[9];
    sprintf(command_name, "letter_%c", letter);

    drawing_command_t command = (drawing_command_t)INLINE_MALLOC(drawing_command,
            .operation_name = command_name,
            .command_data = INLINE_MALLOC(letter_command_data_t,
                .letter = letter,
                .options = opts
            )
        );

    return command;
}

/*
drawing_register_operation("letter_a", letter_draw, letter_bbox, INLINE_MALLOC(letter_operation_data_t, .letter = 'A', .bitmap = "a.bmp"));
drawing_register_operation("letter_b", letter_draw, letter_bbox, INLINE_MALLOC(letter_operation_data_t, .letter = 'A', .bitmap = "a.bmp"));
drawing_register_operation("letter_c", letter_draw, letter_bbox, INLINE_MALLOC(letter_operation_data_t, .letter = 'A', .bitmap = "a.bmp"));
*/