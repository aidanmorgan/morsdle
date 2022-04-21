add_library(waveshare-display)

add_dependencies(
        waveshare-display
        morsdle-util
        morsdle-game
        morsdle-gfx
)

target_include_directories(
        waveshare-display
        PUBLIC
        include
)

target_sources(
        waveshare-display
        PUBLIC
        waveshare_display.c
)

target_link_libraries(
        waveshare-display
        morsdle-util
        morsdle-game
        morsdle-gfx
)

