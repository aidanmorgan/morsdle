//
// Created by aidan on 20/04/2022.
//

#include "morsdle.h"


// TODO : consider if these should actually be added to the game struct rather than being global
static morsdle_game_event_t event_buffer_storage[EVENTS_PER_GAME];
static cbuff_t event_buffer = (cbuff_t)&(struct cbuff){};

static bool morsdle_append_event(morsdle_game_t* game, morsdle_game_event_t* event) {
    return cbuff_write(game->events, event);
}

static bool morsdle_clear_events(morsdle_game_t* game) {
    return cbuff_clear(game->events);
}

void morsdle_init_game(morsdle_game_t* game) {
    game->state = GAME_STATE_IN_PROGRESS;

    for(uint8_t w = 0; w < WORDS_PER_GAME; w++) {
        morsdle_word_t* word = &game->answers[w];
        word->state = w == 0 ? WORD_STATE_IN_PROGRESS : WORD_STATE_NEW;
        word->y = w;

        for(uint8_t l = 0; l< LETTERS_PER_WORD;l++) {
            morsdle_letter_t* letter = &word->letters[l];

            letter->state = LETTER_STATE_UNSET;
            letter->letter = NULL_CHAR;
            letter->x = l;
            letter->y = w;
        }
    }

    cbuff_init(event_buffer, (void**)&event_buffer_storage, EVENTS_PER_GAME, sizeof(morsdle_game_event_t));
    game->events = event_buffer;

    morsdle_append_event(game, &(morsdle_game_event_t) {
            .type = EVENT_GAME_CREATED
    });
    morsdle_append_event(game, &(morsdle_game_event_t) {
            .type = EVENT_WORD_STARTED,
            .word = &game->answers[0]
    });
}

void morsdle_clear(morsdle_game_t* game) {
    game->state = GAME_STATE_IN_PROGRESS;
    morsdle_clear_events(game);

    for(uint8_t w = 0; w < WORDS_PER_GAME; w++) {
        morsdle_word_t*  word = &game->answers[w];
        word->state = WORD_STATE_NEW;

        for(uint8_t l = 0; l < LETTERS_PER_WORD; l++) {
            morsdle_letter_t*  letter = &word->letters[l];

            letter->letter = NULL_CHAR;
            letter->state = LETTER_STATE_UNSET;
        }
    }
}

static morsdle_word_t* get_next_word(morsdle_game_t* game, word_state_t state) {
    for(int8_t i = 0; i < WORDS_PER_GAME; i++) {
        if(game->answers[i].state == state) {
            return &game->answers[i];
        }
    }

    return NULL;
}

static morsdle_letter_t* get_next_letter(morsdle_word_t* word, letter_state_t state) {
    for(int8_t i = 0; i < LETTERS_PER_WORD; i++) {
        if(word->letters[i].state == state) {
            return &word->letters[i];
        }
    }

    return NULL;
}

static morsdle_letter_t* get_last_letter(morsdle_word_t* word, letter_state_t state) {
    for(uint8_t i = LETTERS_PER_WORD; i > 0 ; i--) {
        if(word->letters[i-1].state == state) {
            return &word->letters[i-1];
        }
    }

    return NULL;
}

morsdle_err_t morsdle_add_letter(morsdle_game_t* game, char l) {
    if(game->state != GAME_STATE_IN_PROGRESS) {
        return MORSDLE_ERR_GAMENOTINPROGRESS;
    }

    morsdle_word_t* word = get_next_word(game, WORD_STATE_IN_PROGRESS);
    if(word == NULL) {
        return MORSDLE_ERR_NOINPROGRESSWORD;
    }

    morsdle_letter_t* letter = get_next_letter(word, LETTER_STATE_UNSET);
    if(letter == NULL) {
        return MORSDLE_ERR_NOINPROGRESSLETTER;
    }

    letter->letter = l;
    letter->state = LETTER_STATE_SET;

    morsdle_game_event_t event = (morsdle_game_event_t) {
        .type = EVENT_LETTER_ADDED,
        .word = word,
        .letter = letter
    };

    morsdle_append_event(game, &event);

    return MORSDLE_OK;
}

morsdle_err_t morsdle_submit_word(morsdle_game_t* game) {
    if(game->state != GAME_STATE_IN_PROGRESS) {
        return MORSDLE_ERR_GAMENOTINPROGRESS;
    }

    morsdle_word_t* word = get_next_word(game, WORD_STATE_IN_PROGRESS);
    if(word == NULL) {
        return MORSDLE_ERR_NOINPROGRESSWORD;
    }

    // this will look for the next letter in the word that is UNSET, returning NULL means we have no unset letters
    // which means we are okay to submit the word for checking
    morsdle_letter_t* letter = get_next_letter(word, LETTER_STATE_UNSET);
    if(letter != NULL) {
        return MORSDLE_ERR_WORDINCOMPLETE;
    }

    uint8_t validcount = 0;
    // go through each letter
    for(uint8_t i = 0; i < LETTERS_PER_WORD; i++) {
        // right letter, right postion
        if(word->letters[i].letter == game->word[i]) {
            word->letters[i].state = LETTER_STATE_VALID;
            validcount++;
        }
        // right letter, wrong position
        else if(strstr(game->word, &word->letters[i].letter) != NULL) {
            word->letters[i].state = LETTER_STATE_VALID_LETTER_INVALID_POSITION;
        }
        // wrong letter
        else {
            word->letters[i].state = LETTER_STATE_INVALID_LETTER;
        }
    }

    morsdle_game_event_t event = (morsdle_game_event_t) {
            .type = EVENT_WORD_COMPLETED,
            .word = word,
    };
    morsdle_append_event(game, &event);

    if(validcount == LETTERS_PER_WORD) {
        word->state = WORD_STATE_CORRECT;
        game->state = GAME_STATE_SUCCESS;

        morsdle_game_event_t event = (morsdle_game_event_t) {
                .type = EVENT_GAME_COMPLETED
        };
        morsdle_append_event(game, &event);
    }
    else {
        word->state = WORD_STATE_COMPLETE;
        // get the next word and update it's state to start filling it, if it's null then we're at the end of the
        // list and the game is over - you failed.
        morsdle_word_t* nextword = get_next_word(game, WORD_STATE_NEW);
        if (nextword == NULL) {
            game->state = GAME_STATE_FAILED;

            morsdle_game_event_t event = (morsdle_game_event_t) {
                    .type = EVENT_GAME_COMPLETED
            };
            morsdle_append_event(game, &event);
        } else {
            nextword->state = WORD_STATE_IN_PROGRESS;

            morsdle_game_event_t event = (morsdle_game_event_t) {
                    .type = EVENT_WORD_STARTED,
                    .word = word
            };
            morsdle_append_event(game, &event);
        }
    }

    return MORSDLE_OK;
}

morsdle_err_t morsdle_remove_letter(morsdle_game_t* game) {
    if(game->state != GAME_STATE_IN_PROGRESS) {
        return MORSDLE_ERR_GAMENOTINPROGRESS;
    }

    morsdle_word_t* word = get_next_word(game, WORD_STATE_IN_PROGRESS);
    if(word == NULL) {
        return MORSDLE_ERR_NOINPROGRESSWORD;
    }

    morsdle_letter_t* letter = get_last_letter(word, LETTER_STATE_SET);
    if(letter == NULL) {
        return MORSDLE_ERR_NOSETLETTER;
    }

    letter->letter = NULL_CHAR;
    letter->state = LETTER_STATE_UNSET;

    morsdle_game_event_t event = (morsdle_game_event_t) {
            .type = EVENT_LETTER_REMOVED,
            .word = word,
            .letter = letter
    };
    morsdle_append_event(game, &event);

    return MORSDLE_OK;
}



bool morsdle_has_events(morsdle_game_t* game) {
    return cbuff_canread(game->events);
}

bool morsdle_read_event(morsdle_game_t* game, morsdle_game_event_t * event) {
    return cbuff_read(game->events, event);
}

