
add_library(mock-display)
add_dependencies(
        mock-display
        morsdle-util
        morsdle-gfx
)

target_include_directories(
        mock-display
        PUBLIC
        include
)

target_sources(
        mock-display
        PUBLIC
        mock_display.c
)

target_link_libraries(
        mock-display
        morsdle-util
        morsdle-gfx
)