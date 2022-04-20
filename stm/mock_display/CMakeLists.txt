
add_library(morsdle-mock-display)

target_include_directories(
        morsdle-mock-display
        PUBLIC
        include
)

target_sources(
        morsdle-mock-display
        PUBLIC
        mock_display.c
)

target_link_libraries(morsdle-core)