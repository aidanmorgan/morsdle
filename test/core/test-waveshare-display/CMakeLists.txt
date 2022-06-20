add_executable(test-waveshare-display)

add_dependencies(
        test-gfx
        unity
        svg
        morsdle-util
        morsdle-game
        morsdle-gfx
        waveshare-display
)

target_include_directories(
        test-waveshare-display
        PUBLIC
        include
)

target_sources(
        test-waveshare-display
        PUBLIC
        test_waveshare_display.c
)

target_link_libraries(
        test-waveshare-display
        unity
        svg
        morsdle-util
        morsdle-game
        morsdle-gfx
        waveshare-display
)

add_test(
        NAME test-waveshare-display
        COMMAND test-waveshare-display
)
