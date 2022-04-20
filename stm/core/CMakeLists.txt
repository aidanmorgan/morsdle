
add_library(morsdle-core)

target_include_directories(
        morsdle-core
        PUBLIC
        include
        include/operations
)

target_sources(
        morsdle-core
        PUBLIC
        drawing.c
        operations/letter_operations.c
        operations/line_operations.c
)