//
// Created by aidan on 20/04/2022.
//

#include <ctype.h>
#include "morsdle.h"


// TODO : consider if these should actually be added to the game struct rather than being global
static morsdle_game_event_t event_buffer_storage[EVENT_BUFFER_SIZE];
static cbuff_t *event_buffer = &(cbuff_t) {};

static morsdle_letter_t letters[WORDS_PER_GAME * LETTERS_PER_WORD];
static morsdle_word_t words[WORDS_PER_GAME];

static bool morsdle_append_event(morsdle_game_t *game, morsdle_game_event_t *event) {
    return cbuff_write(game->events, event);
}

static bool morsdle_clear_events(morsdle_game_t *game) {
    return cbuff_clear(game->events);
}

static inline void force_uppercase(char* word) {
    char *s = word;
    while(s) {
        *s = toupper((unsigned char) *s);
        s++;
    }
}

void morsdle_init_game(morsdle_game_t *game, char *word) {
    cbuff_init(event_buffer, (void **) &event_buffer_storage, EVENT_BUFFER_SIZE, sizeof(morsdle_game_event_t));
    game->events = event_buffer;
    game->word = word;

    morsdle_clear(game);

    morsdle_append_event(game, &(morsdle_game_event_t) {
            .type = EVENT_WORD_STARTED,
            .word = game->answers[0]
    });
}

void morsdle_clear(morsdle_game_t *game) {
    game->state = GAME_STATE_IN_PROGRESS;
    morsdle_clear_events(game);

    for (uint8_t idx = 0; idx < (WORDS_PER_GAME * LETTERS_PER_WORD); idx++) {
        morsdle_letter_t *letter = &letters[idx];
        morsdle_word_t *word = &words[idx / LETTERS_PER_WORD];

        letter->state = LETTER_STATE_UNSET;
        letter->x = idx % LETTERS_PER_WORD;
        letter->y = idx / LETTERS_PER_WORD;
        letter->letter = NULL_CHAR;

        word->y = idx / LETTERS_PER_WORD;
        word->state = idx / LETTERS_PER_WORD == 0 ? WORD_STATE_IN_PROGRESS : WORD_STATE_NEW;
        word->letters[idx % LETTERS_PER_WORD] = letter;

        game->answers[idx / LETTERS_PER_WORD] = word;
    }

    // given we've cleared the board, make sure we add the event to say a new game has been created so we redraw as required.
    morsdle_game_event_t ev = (morsdle_game_event_t) {
            .type = EVENT_GAME_CREATED
    };

    morsdle_append_event(game, &ev);
}

static morsdle_word_t *get_next_word(morsdle_game_t *game, word_state_t state) {
    for (int8_t i = 0; i < WORDS_PER_GAME; i++) {
        if (game->answers[i]->state == state) {
            return game->answers[i];
        }
    }

    return NULL;
}

static morsdle_letter_t *get_next_letter(morsdle_word_t *word, letter_state_t state) {
    for (int8_t i = 0; i < LETTERS_PER_WORD; i++) {
        if (word->letters[i]->state == state) {
            return word->letters[i];
        }
    }

    return NULL;
}

static morsdle_letter_t *get_last_letter(morsdle_word_t *word, letter_state_t state) {
    for (uint8_t i = LETTERS_PER_WORD; i > 0; i--) {
        if (word->letters[i - 1]->state == state) {
            return word->letters[i - 1];
        }
    }

    return NULL;
}

morsdle_err_t morsdle_add_letter(morsdle_game_t *game, char l) {
    if (game->state != GAME_STATE_IN_PROGRESS) {
        return MORSDLE_ERR_GAMENOTINPROGRESS;
    }

    morsdle_word_t *word = get_next_word(game, WORD_STATE_IN_PROGRESS);
    if (word == NULL) {
        return MORSDLE_ERR_NOINPROGRESSWORD;
    }

    morsdle_letter_t *letter = get_next_letter(word, LETTER_STATE_UNSET);
    if (letter == NULL) {
        return MORSDLE_ERR_NOINPROGRESSLETTER;
    }

    letter->letter = toupper(l);
    letter->state = LETTER_STATE_SET;

    morsdle_game_event_t event = (morsdle_game_event_t) {
            .type = EVENT_LETTER_ADDED,
            .word = word,
            .letter = letter
    };

    morsdle_append_event(game, &event);

    if(game->auto_submit) {
        morsdle_submit_word(game);
    }

    return MORSDLE_OK;
}

morsdle_err_t morsdle_submit_word(morsdle_game_t *game) {
    if (game->state != GAME_STATE_IN_PROGRESS) {
        return MORSDLE_ERR_GAMENOTINPROGRESS;
    }

    morsdle_word_t *word = get_next_word(game, WORD_STATE_IN_PROGRESS);
    if (word == NULL) {
        return MORSDLE_ERR_NOINPROGRESSWORD;
    }

    // this will look for the next letter in the word that is UNSET, returning NULL means we have no unset letters
    // which means we are okay to submit the word for checking
    morsdle_letter_t *letter = get_next_letter(word, LETTER_STATE_UNSET);
    if (letter != NULL) {
        return MORSDLE_ERR_WORDINCOMPLETE;
    }

    uint8_t validcount = 0;
    // go through each letter
    for (uint8_t i = 0; i < LETTERS_PER_WORD; i++) {
        morsdle_letter_t *letter = word->letters[i];

        // right letter, right postion
        if (letter->letter == game->word[i]) {
            letter->state = LETTER_STATE_VALID;
            validcount++;
        }
            // right letter, wrong position
        else if (strstr(game->word, &letter->letter) != NULL) {
            letter->state = LETTER_STATE_VALID_LETTER_INVALID_POSITION;
        }
            // wrong letter
        else {
            letter->state = LETTER_STATE_INVALID_LETTER;
        }
    }

    morsdle_game_event_t event = (morsdle_game_event_t) {
            .type = EVENT_WORD_COMPLETED,
            .word = word,
    };
    morsdle_append_event(game, &event);

    if (validcount == LETTERS_PER_WORD) {
        word->state = WORD_STATE_CORRECT;
        game->state = GAME_STATE_SUCCESS;

        event = (morsdle_game_event_t) {
                .type = EVENT_GAME_COMPLETED
        };
        morsdle_append_event(game, &event);
    } else {
        word->state = WORD_STATE_COMPLETE;
        // get the next word and update it's state to start filling it, if it's null then we're at the end of the
        // list and the game is over - you failed.
        morsdle_word_t *nextword = get_next_word(game, WORD_STATE_NEW);
        if (nextword == NULL) {
            game->state = GAME_STATE_FAILED;

            event = (morsdle_game_event_t) {
                    .type = EVENT_GAME_COMPLETED
            };
            morsdle_append_event(game, &event);
        } else {
            nextword->state = WORD_STATE_IN_PROGRESS;

            event = (morsdle_game_event_t) {
                    .type = EVENT_WORD_STARTED,
                    .word = word
            };
            morsdle_append_event(game, &event);
        }
    }

    return MORSDLE_OK;
}

morsdle_err_t morsdle_remove_letter(morsdle_game_t *game) {
    if (game->state != GAME_STATE_IN_PROGRESS) {
        return MORSDLE_ERR_GAMENOTINPROGRESS;
    }

    morsdle_word_t *word = get_next_word(game, WORD_STATE_IN_PROGRESS);
    if (word == NULL) {
        return MORSDLE_ERR_NOINPROGRESSWORD;
    }

    morsdle_letter_t *letter = get_last_letter(word, LETTER_STATE_SET);
    if (letter == NULL) {
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


bool morsdle_has_events(morsdle_game_t *game) {
    return cbuff_canread(game->events);
}

bool morsdle_read_event(morsdle_game_t *game, morsdle_game_event_t *event) {
    return cbuff_read(game->events, event);
}

