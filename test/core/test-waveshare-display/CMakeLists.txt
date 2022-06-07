add_executable(test-waveshare-display)

add_dependencies(
        test-waveshare-display
        unity
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
        waveshare-display
)

add_test(
        NAME test-waveshare-display
        COMMAND test-waveshare-display
)
