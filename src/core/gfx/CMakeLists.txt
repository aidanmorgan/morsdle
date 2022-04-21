
add_library(morsdle-gfx)

add_dependencies(
        morsdle-gfx
        morsdle-util
)

target_include_directories(
        morsdle-gfx
        PUBLIC
        include
        include/operations
)

target_sources(
        morsdle-gfx
        PUBLIC
        drawing.c
        operations/letter_operations.c
        operations/line_operations.c
)

target_link_libraries(
        morsdle-gfx
        morsdle-util
)