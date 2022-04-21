//
// Created by aidan on 21/04/2022.
//
#include "unity.h"
#include "../../../src/core/game/include/morsdle.h"

void test_basic_init() {
    morsdle_game game = (morsdle_game) {.word = "ratio" };
    morsdle_init_game(&game);

    TEST_ASSERT_TRUE(game.state == GAME_STATE_IN_PROGRESS);
    TEST_ASSERT_EQUAL_STRING("ratio", game.word);
    TEST_ASSERT_NOT_NULL(game.answers);


}


int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_basic_init);
    return UNITY_END();
}

