#include "drawing.h"
#include "operations/letter_operations.h"


#define LETTER_WIDTH_PX 50U
#define LETTER_HEIGHT_PX 50U

struct letter_operation_data_t {
    char letter;
    char* bitmap;
};

struct letter_command_data_t{
    char letter;
    float opacity;
} ;

drawing_operation_error_t letter_draw(display_surface_t surface, struct drawing_rectangle_t bbox, void*operation_data, void*command_data) {
    struct letter_operation_data_t * data = (struct letter_operation_data_t*)operation_data;
    // we know what letter we have, but how the fuck do we render it

    // do drawing process
    return DRAWING_OPERATION_OK;
}

void letter_bbox(struct drawing_rectangle_t* bbox, void* operation_data, void*command_data) {
    bbox->top_left = (struct drawing_point_t){ .x = 0, .y = 0};
    bbox->bottom_right = (struct drawing_point_t){ .x = LETTER_WIDTH_PX, .y = LETTER_HEIGHT_PX};
}

drawing_command_t letter_create(char letter, float opacity) {
    // dodgy, I know, but we know that the name is fixed as there are only 26 letters in the alphabet
    char command_name[9];
    sprintf(command_name, "letter_%c", letter);

    drawing_command_t command = INLINE_MALLOC(struct drawing_command_instance_t,
            .name = command_name,
            .command_data = INLINE_MALLOC(struct letter_command_data_t,
                .letter = letter,
                .opacity = opacity
    ));

    return (drawing_command_t)command;
}

REGISTER_DRAWING_OPERATION_WITH_DATA("letter_a", letter_draw, letter_bbox, ((struct letter_operation_data_t) {.letter = 'A', .bitmap = "a.bmp"}))
REGISTER_DRAWING_OPERATION_WITH_DATA("letter_b", letter_draw, letter_bbox, ((struct letter_operation_data_t) {.letter = 'B', .bitmap = "b.bmp"}))
REGISTER_DRAWING_OPERATION_WITH_DATA("letter_c", letter_draw, letter_bbox, ((struct letter_operation_data_t) {.letter = 'C', .bitmap = "c.bmp"}))