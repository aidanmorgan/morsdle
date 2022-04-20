
add_library(morsdle-core-lib)

target_include_directories(
        morsdle-core-lib
        PUBLIC
        include
        include/operations
)

target_sources(
        morsdle-core-lib
        PUBLIC
        drawing.c
        operations/letter_operations.c
        operations/line_operations.c
        )