add_executable(test-gfx)

add_dependencies(
        test-gfx
        unity
        svg
        morsdle-util
        morsdle-game
        morsdle-gfx
)

target_include_directories(
        test-gfx
        PUBLIC
        include
)

target_sources(
        test-gfx
        PUBLIC
        mock_display.c
        test_renderer.c
)

target_link_libraries(
        test-gfx
        unity
        svg
        morsdle-util
        morsdle-game
        morsdle-gfx
)

