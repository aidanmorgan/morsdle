//
// Created by aidan on 20/04/2022.
//

#ifndef __MORSDLE_H__
#define __MORSDLE_H__

#include "allocations.h"
#include "cbuff.h"

#include <stdint.h>

#define LETTERS_PER_WORD 5
#define WORDS_PER_GAME 6
#define EVENT_BUFFER_SIZE 16

#define NULL_CHAR (char)0

typedef enum {
    MORSDLE_OK,
    MORSDLE_ERR_GAMENOTINPROGRESS,
    MORSDLE_ERR_NOINPROGRESSWORD,
    MORSDLE_ERR_NOINPROGRESSLETTER,
    MORSDLE_ERR_WORDINCOMPLETE,
    MORSDLE_ERR_NOSETLETTER
} morsdle_err_t;

typedef enum {
    LETTER_STATE_UNSET,
    LETTER_STATE_SET,

    LETTER_STATE_VALID,
    LETTER_STATE_VALID_LETTER_INVALID_POSITION,
    LETTER_STATE_INVALID_LETTER
} letter_state_t;

typedef enum {
    WORD_STATE_NEW,
    WORD_STATE_IN_PROGRESS,
    WORD_STATE_COMPLETE,
    WORD_STATE_CORRECT
} word_state_t;

typedef enum {
    GAME_STATE_IN_PROGRESS,
    GAME_STATE_FAILED,
    GAME_STATE_SUCCESS

} game_state_t;

typedef enum {
    EVENT_NONE,
    EVENT_GAME_CREATED,
    EVENT_LETTER_ADDED,
    EVENT_LETTER_REMOVED,
    EVENT_WORD_STARTED,
    EVENT_WORD_COMPLETED,
    EVENT_GAME_COMPLETED
} game_event_t;

typedef struct {
    char letter;
    letter_state_t state;

    uint8_t x;
    uint8_t y;

} morsdle_letter_t;

typedef struct {
    word_state_t state;
    uint8_t y;
    morsdle_letter_t *letters[LETTERS_PER_WORD];
} morsdle_word_t;

typedef struct {
    game_event_t type;
    morsdle_word_t *word;
    morsdle_letter_t *letter;
} morsdle_game_event_t;

typedef struct {
    const char *word;
    game_state_t state;
    cbuff_t *events;

    morsdle_word_t *answers[WORDS_PER_GAME];
} morsdle_game_t;

void morsdle_init_game(morsdle_game_t *game, const char *word);

morsdle_err_t morsdle_add_letter(morsdle_game_t *game, char letter);

morsdle_err_t morsdle_remove_letter(morsdle_game_t *game);

morsdle_err_t morsdle_submit_word(morsdle_game_t *game);

void morsdle_clear(morsdle_game_t *game);

// event management stuff here
bool morsdle_read_event(morsdle_game_t *game, morsdle_game_event_t *event);

bool morsdle_has_events(morsdle_game_t *game);

#endif //__MORSDLE_H__
