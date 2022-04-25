add_library(svg)

target_include_directories(
        svg
        PUBLIC
        include
)

target_sources(
        svg
        PUBLIC
        svg.c
)