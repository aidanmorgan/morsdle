add_library(morsdle-gfx)

add_dependencies(
        morsdle-gfx
        morsdle-util
        morsdle-game
)

target_include_directories(
        morsdle-gfx
        PUBLIC
        include
)

target_sources(
        morsdle-gfx
        PUBLIC
        renderer.c
        display.c
        render_pass.c
)

target_link_libraries(
        morsdle-gfx
        morsdle-util
        morsdle-game
)