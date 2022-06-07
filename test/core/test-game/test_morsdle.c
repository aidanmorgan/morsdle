//
// Created by aidan on 21/04/2022.
//
#include "unity.h"
#include "morsdle.h"

void setUp (void) {} /* Is run before every test, put unit init calls here. */
void tearDown (void) {} /* Is run after every test, put unit clean-up calls here. */

static void assert_eventsequence(morsdle_game_t* game, game_event_t* events, uint8_t length) {
    for(uint8_t i = 0; i < length; i++) {
        morsdle_game_event_t ev;
        morsdle_read_event(game, &ev);

        game_event_t expected = events[i];
        TEST_ASSERT_EQUAL(expected, ev.type);
    }
}

void test_basic_init() {
    morsdle_game_t game = (morsdle_game_t) {.word = "ratio" };
    morsdle_init_game(&game);

    TEST_ASSERT_TRUE(game.state == GAME_STATE_IN_PROGRESS);
    TEST_ASSERT_EQUAL_STRING("ratio", game.word);

    TEST_ASSERT_EQUAL(WORD_STATE_IN_PROGRESS, game.answers[0]->state);
    TEST_ASSERT_EQUAL(WORD_STATE_NEW, game.answers[1]->state);
    TEST_ASSERT_EQUAL(WORD_STATE_NEW, game.answers[2]->state);
    TEST_ASSERT_EQUAL(WORD_STATE_NEW, game.answers[3]->state);
    TEST_ASSERT_EQUAL(WORD_STATE_NEW, game.answers[4]->state);
    TEST_ASSERT_EQUAL(WORD_STATE_NEW, game.answers[5]->state);

    game_event_t events[2] = {EVENT_GAME_CREATED, EVENT_WORD_STARTED};
    assert_eventsequence(&game, events, 2);

}

void test_add_letter() {
    morsdle_game_t game = (morsdle_game_t) {.word = "ratio" };
    morsdle_init_game(&game);

    morsdle_add_letter(&game, 'b');
    TEST_ASSERT_EQUAL('b', game.answers[0]->letters[0]->letter);
    TEST_ASSERT_EQUAL(LETTER_STATE_SET, game.answers[0]->letters[0]->state);

    morsdle_add_letter(&game, 'u');
    TEST_ASSERT_EQUAL('u', game.answers[0]->letters[1]->letter);

    morsdle_add_letter(&game, 't');
    TEST_ASSERT_EQUAL('t', game.answers[0]->letters[2]->letter);

    morsdle_add_letter(&game, 't');
    TEST_ASSERT_EQUAL('t', game.answers[0]->letters[3]->letter);

    morsdle_add_letter(&game, 's');
    TEST_ASSERT_EQUAL('s', game.answers[0]->letters[4]->letter);


    game_event_t events[7] = {EVENT_GAME_CREATED,
                              EVENT_WORD_STARTED,
                              EVENT_LETTER_ADDED,
                              EVENT_LETTER_ADDED,
                              EVENT_LETTER_ADDED,
                              EVENT_LETTER_ADDED,
                              EVENT_LETTER_ADDED};
    assert_eventsequence(&game, events, 7);
}

void test_add_letter_alreadyfiveletters_shouldreturnerror() {
    morsdle_game_t game = (morsdle_game_t) {.word = "ratio" };
    morsdle_init_game(&game);

    TEST_ASSERT_EQUAL(MORSDLE_OK, morsdle_add_letter(&game, 'b'));
    TEST_ASSERT_EQUAL(MORSDLE_OK, morsdle_add_letter(&game, 'u'));
    TEST_ASSERT_EQUAL(MORSDLE_OK, morsdle_add_letter(&game, 't'));
    TEST_ASSERT_EQUAL(MORSDLE_OK, morsdle_add_letter(&game, 't'));
    TEST_ASSERT_EQUAL(MORSDLE_OK, morsdle_add_letter(&game, 's'));

    morsdle_err_t result = morsdle_add_letter(&game, 'e');
    TEST_ASSERT_EQUAL(MORSDLE_ERR_NOINPROGRESSLETTER, result);
}

void test_remove_letter() {
    morsdle_game_t game = (morsdle_game_t) {.word = "ratio"};
    morsdle_init_game(&game);

    TEST_ASSERT_EQUAL(MORSDLE_OK, morsdle_add_letter(&game, 'b'));
    TEST_ASSERT_EQUAL(LETTER_STATE_SET, game.answers[0]->letters[0]->state);
    TEST_ASSERT_EQUAL('b', game.answers[0]->letters[0]->letter);

    TEST_ASSERT_EQUAL(MORSDLE_OK, morsdle_remove_letter(&game));
    TEST_ASSERT_EQUAL(LETTER_STATE_UNSET, game.answers[0]->letters[0]->state);
    TEST_ASSERT_EQUAL((char)0, game.answers[0]->letters[0]->letter);

    TEST_ASSERT_EQUAL(MORSDLE_OK, morsdle_add_letter(&game, 'b'));
    TEST_ASSERT_EQUAL(MORSDLE_OK, morsdle_add_letter(&game, 'u'));
    TEST_ASSERT_EQUAL(MORSDLE_OK, morsdle_add_letter(&game, 't'));
    TEST_ASSERT_EQUAL(MORSDLE_OK, morsdle_add_letter(&game, 't'));
    TEST_ASSERT_EQUAL(MORSDLE_OK, morsdle_add_letter(&game, 's'));

    TEST_ASSERT_EQUAL(MORSDLE_OK, morsdle_remove_letter(&game));
    TEST_ASSERT_EQUAL(MORSDLE_OK, morsdle_remove_letter(&game));
    TEST_ASSERT_EQUAL(MORSDLE_OK, morsdle_remove_letter(&game));

    TEST_ASSERT_EQUAL(LETTER_STATE_SET, game.answers[0]->letters[0]->state);
    TEST_ASSERT_EQUAL(LETTER_STATE_SET, game.answers[0]->letters[1]->state);
    TEST_ASSERT_EQUAL(LETTER_STATE_UNSET, game.answers[0]->letters[2]->state);
    TEST_ASSERT_EQUAL(LETTER_STATE_UNSET, game.answers[0]->letters[3]->state);
    TEST_ASSERT_EQUAL(LETTER_STATE_UNSET, game.answers[0]->letters[4]->state);

    game_event_t events[12] = {EVENT_GAME_CREATED,
                               EVENT_WORD_STARTED,
                               EVENT_LETTER_ADDED, // b
                               EVENT_LETTER_REMOVED, // b
                               EVENT_LETTER_ADDED, // b
                               EVENT_LETTER_ADDED, // u
                               EVENT_LETTER_ADDED, // t
                               EVENT_LETTER_ADDED, // t
                               EVENT_LETTER_ADDED, // s
                               EVENT_LETTER_REMOVED, // s
                               EVENT_LETTER_REMOVED, // t
                               EVENT_LETTER_REMOVED}; // t
    assert_eventsequence(&game, events, 12);
}

void test_remove_letter_nolettertoremove_shouldreturnerror() {
    morsdle_game_t game = (morsdle_game_t) {.word = "ratio" };
    morsdle_init_game(&game);

    morsdle_err_t result = morsdle_remove_letter(&game);
    TEST_ASSERT_EQUAL(MORSDLE_ERR_NOSETLETTER, result);
}

void test_submit_word() {
    morsdle_game_t game = (morsdle_game_t) {.word = "ratio" };
    morsdle_init_game(&game);

    TEST_ASSERT_EQUAL(MORSDLE_OK, morsdle_add_letter(&game, 'b'));
    TEST_ASSERT_EQUAL(MORSDLE_OK, morsdle_add_letter(&game, 'u'));
    TEST_ASSERT_EQUAL(MORSDLE_OK, morsdle_add_letter(&game, 't'));
    TEST_ASSERT_EQUAL(MORSDLE_OK, morsdle_add_letter(&game, 't'));
    TEST_ASSERT_EQUAL(MORSDLE_OK, morsdle_add_letter(&game, 's'));

    morsdle_err_t result = morsdle_submit_word(&game);
    TEST_ASSERT_EQUAL(MORSDLE_OK, result);
    TEST_ASSERT_EQUAL(WORD_STATE_COMPLETE, game.answers[0]->state);
    TEST_ASSERT_EQUAL(WORD_STATE_IN_PROGRESS, game.answers[1]->state);

    TEST_ASSERT_EQUAL(LETTER_STATE_INVALID_LETTER, game.answers[0]->letters[0]->state);
    TEST_ASSERT_EQUAL(LETTER_STATE_INVALID_LETTER, game.answers[0]->letters[1]->state);
    TEST_ASSERT_EQUAL(LETTER_STATE_VALID, game.answers[0]->letters[2]->state);
    TEST_ASSERT_EQUAL(LETTER_STATE_VALID_LETTER_INVALID_POSITION, game.answers[0]->letters[3]->state);
    TEST_ASSERT_EQUAL(LETTER_STATE_INVALID_LETTER, game.answers[0]->letters[4]->state);

    game_event_t events[9] = {EVENT_GAME_CREATED,
                               EVENT_WORD_STARTED,
                               EVENT_LETTER_ADDED, // b
                               EVENT_LETTER_ADDED, // u
                               EVENT_LETTER_ADDED, // t
                               EVENT_LETTER_ADDED, // t
                               EVENT_LETTER_ADDED, // s
                               EVENT_WORD_COMPLETED,
                               EVENT_WORD_STARTED};
    assert_eventsequence(&game, events, 9);
}

void test_submit_word_notenoughletters_shouldreturnerror() {
    morsdle_game_t game = (morsdle_game_t) {.word = "ratio" };
    morsdle_init_game(&game);

    TEST_ASSERT_EQUAL(MORSDLE_OK, morsdle_add_letter(&game, 'b'));
    TEST_ASSERT_EQUAL(MORSDLE_OK, morsdle_add_letter(&game, 'u'));
    TEST_ASSERT_EQUAL(MORSDLE_OK, morsdle_add_letter(&game, 't'));
    TEST_ASSERT_EQUAL(MORSDLE_OK, morsdle_add_letter(&game, 't'));

    morsdle_err_t result = morsdle_submit_word(&game);
    TEST_ASSERT_EQUAL(MORSDLE_ERR_WORDINCOMPLETE, result);
    TEST_ASSERT_EQUAL(WORD_STATE_IN_PROGRESS, game.answers[0]->state);
}

void test_submit_word_correctanswer() {
    morsdle_game_t game = (morsdle_game_t) {.word = "ratio" };
    morsdle_init_game(&game);

    TEST_ASSERT_EQUAL(MORSDLE_OK, morsdle_add_letter(&game, 'r'));
    TEST_ASSERT_EQUAL(MORSDLE_OK, morsdle_add_letter(&game, 'a'));
    TEST_ASSERT_EQUAL(MORSDLE_OK, morsdle_add_letter(&game, 't'));
    TEST_ASSERT_EQUAL(MORSDLE_OK, morsdle_add_letter(&game, 'i'));
    TEST_ASSERT_EQUAL(MORSDLE_OK, morsdle_add_letter(&game, 'o'));

    TEST_ASSERT_EQUAL(MORSDLE_OK, morsdle_submit_word(&game));
    TEST_ASSERT_EQUAL(GAME_STATE_SUCCESS, game.state);
    TEST_ASSERT_EQUAL(WORD_STATE_CORRECT, game.answers[0]->state);

    game_event_t events[9] = {EVENT_GAME_CREATED,
                              EVENT_WORD_STARTED,
                              EVENT_LETTER_ADDED, // r
                              EVENT_LETTER_ADDED, // a
                              EVENT_LETTER_ADDED, // t
                              EVENT_LETTER_ADDED, // i
                              EVENT_LETTER_ADDED, // o
                              EVENT_WORD_COMPLETED,
                              EVENT_GAME_COMPLETED};
    assert_eventsequence(&game, events, 9);
}

void test_submit_word_incorrectanswer() {
    morsdle_game_t game = (morsdle_game_t) {.word = "ratio" };
    morsdle_init_game(&game);

    for(uint8_t i = 0; i < WORDS_PER_GAME; i++) {
        TEST_ASSERT_EQUAL(MORSDLE_OK, morsdle_add_letter(&game, 'b'));
        TEST_ASSERT_EQUAL(MORSDLE_OK, morsdle_add_letter(&game, 'u'));
        TEST_ASSERT_EQUAL(MORSDLE_OK, morsdle_add_letter(&game, 't'));
        TEST_ASSERT_EQUAL(MORSDLE_OK, morsdle_add_letter(&game, 't'));
        TEST_ASSERT_EQUAL(MORSDLE_OK, morsdle_add_letter(&game, 's'));

        TEST_ASSERT_EQUAL(MORSDLE_OK, morsdle_submit_word(&game));
    }

    TEST_ASSERT_EQUAL(GAME_STATE_FAILED, game.state);
    TEST_ASSERT_EQUAL(WORD_STATE_COMPLETE, game.answers[0]->state);
    TEST_ASSERT_EQUAL(WORD_STATE_COMPLETE, game.answers[1]->state);
    TEST_ASSERT_EQUAL(WORD_STATE_COMPLETE, game.answers[2]->state);
    TEST_ASSERT_EQUAL(WORD_STATE_COMPLETE, game.answers[3]->state);
    TEST_ASSERT_EQUAL(WORD_STATE_COMPLETE, game.answers[4]->state);
    TEST_ASSERT_EQUAL(WORD_STATE_COMPLETE, game.answers[5]->state);
}


int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_basic_init);
    RUN_TEST(test_add_letter);
    RUN_TEST(test_add_letter_alreadyfiveletters_shouldreturnerror);
    RUN_TEST(test_remove_letter);
    RUN_TEST(test_remove_letter_nolettertoremove_shouldreturnerror);
    RUN_TEST(test_submit_word);
    RUN_TEST(test_submit_word_notenoughletters_shouldreturnerror);
    RUN_TEST(test_submit_word_correctanswer);
    RUN_TEST(test_submit_word_incorrectanswer);
    return UNITY_END();
}

