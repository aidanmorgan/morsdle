add_executable(test-game)

add_dependencies(
        test-game
        unity
        morsdle-util
        morsdle-game
)

target_include_directories(
        test-game
        PUBLIC
        include
)

target_sources(
        test-game
        PUBLIC
        test_morsdle.c
)

target_link_libraries(
        test-game
        unity
        morsdle-util
        morsdle-game
)

add_test(
        NAME test-game
        COMMAND test-game
)


