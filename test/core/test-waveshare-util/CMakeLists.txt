add_executable(test-waveshare-util)

add_dependencies(
        test-waveshare-util
        unity
        waveshare-display
)

target_include_directories(
        test-waveshare-util
        PUBLIC
        include
)

target_sources(
        test-waveshare-util
        PUBLIC
        test_waveshare_imagebuffer.c
)

target_link_libraries(
        test-waveshare-util
        unity
        waveshare-display
)

add_test(
        NAME test-waveshare-util
        COMMAND test-waveshare-util
)
