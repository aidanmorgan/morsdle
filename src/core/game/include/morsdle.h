//
// Created by aidan on 20/04/2022.
//

#ifndef __MORSDLE_H__
#define __MORSDLE_H__

#include "llist.h"
#include "allocations.h"

#define LETTERS_PER_WORD 5
#define WORDS_PER_GAME 6

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
}letter_state_t;

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

} morsdle_letter;
typedef morsdle_letter* morsdle_letter_t;

typedef struct {
    morsdle_letter** letters;
    word_state_t state;

    uint8_t y;
} morsdle_word;
typedef morsdle_word* morsdle_word_t;

typedef struct {
    char * word;
    morsdle_word** answers;
    game_state_t state;
    llist_t events;
} morsdle_game;
typedef morsdle_game* morsdle_game_t;

typedef struct {
    game_event_t type;
    morsdle_game_t game;
    morsdle_word_t word;
    morsdle_letter_t letter;
} game_change_event;
typedef game_change_event* morsdle_game_change_event_t;


void morsdle_init_game(morsdle_game_t game);
morsdle_err_t morsdle_add_letter(morsdle_game_t game, char letter);
morsdle_err_t morsdle_remove_letter(morsdle_game_t game);
morsdle_err_t morsdle_submit_word(morsdle_game_t game);
void morsdle_clear(morsdle_game_t game);


#endif //__MORSDLE_H__
