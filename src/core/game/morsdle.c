//
// Created by aidan on 20/04/2022.
//

#include "include/morsdle.h"

void morsdle_init_game(morsdle_game_t* game) {
    game->state = GAME_STATE_IN_PROGRESS;
    morsdle_clear_events(game);

    for(uint8_t w = 0; w < WORDS_PER_GAME; w++) {
        morsdle_word_t* word = &game->answers[w];
        word->state = w == 0 ? WORD_STATE_IN_PROGRESS : WORD_STATE_NEW;
        word->y = w;

        for(uint8_t l = 0; l< LETTERS_PER_WORD;l++) {
            morsdle_letter_t* letter = &word->letters[l];
            letter->state = LETTER_STATE_UNSET;

            letter->x = l;
            letter->y = w;
        }
    }

    morsdle_game_event_t* event = morsdle_next_event(game);
    event->type = EVENT_GAME_CREATED;
}

void morsdle_clear(morsdle_game_t* game) {
    game->state = GAME_STATE_IN_PROGRESS;
    morsdle_clear_events(game);

    for(uint8_t w = 0; w < WORDS_PER_GAME; w++) {
        morsdle_word_t*  word = &game->answers[w];
        word->state = WORD_STATE_NEW;

        for(uint8_t l = 0; l < LETTERS_PER_WORD; l++) {
            morsdle_letter_t*  letter = &word->letters[l];

            letter->letter = (char)0;
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
    // need to use a signed int as we're working backwards
    for(int8_t i = LETTERS_PER_WORD -1; i >= 0 ; i--) {
        if(word->letters[i].state == state) {
            return &word->letters[i];
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

    morsdle_game_event_t * event = morsdle_next_event(game);
    event->type = EVENT_LETTER_ADDED;
    event->word = word;
    event->letter = letter;

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

    morsdle_game_event_t* ev = morsdle_next_event(game);
    ev->type = EVENT_WORD_COMPLETED;
    ev->word = word;

    if(validcount == LETTERS_PER_WORD) {
        word->state = WORD_STATE_CORRECT;
        game->state = GAME_STATE_SUCCESS;

        ev = morsdle_next_event(game);
        ev->type = EVENT_GAME_COMPLETED;
    }
    else {
        word->state = WORD_STATE_COMPLETE;
        // get the next word and update it's state to start filling it, if it's null then we're at the end of the
        // list and the game is over - you failed.
        morsdle_word_t* nextword = get_next_word(game, WORD_STATE_NEW);
        if (nextword == NULL) {
            game->state = GAME_STATE_FAILED;

            ev = morsdle_next_event(game);
            ev->type = EVENT_GAME_COMPLETED;
        } else {
            nextword->state = WORD_STATE_IN_PROGRESS;

            ev = morsdle_next_event(game);
            ev->type = EVENT_WORD_STARTED;
            ev->word = word;
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

    letter->letter = (char)0;
    letter->state = LETTER_STATE_UNSET;

    morsdle_game_event_t*  ev = morsdle_next_event(game);
    ev->type = EVENT_LETTER_REMOVED;

    return MORSDLE_OK;
}

morsdle_game_event_t* morsdle_next_event(morsdle_game_t* game) {
    morsdle_game_event_t* event = &game->events[game->event_counter++];
    game->event_counter++;
    return event;
}

void morsdle_clear_events(morsdle_game_t* game) {
    game->event_counter = 0;

    for(uint8_t i = 0; i <EVENTS_PER_GAME; i++) {
        morsdle_game_event_t* event = &game->events[i];
        event->type = EVENT_NONE;
        event->word = NULL;
        event->letter = NULL;
    }
}
