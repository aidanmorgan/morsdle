
add_library(morse)

add_dependencies(
        morse
        morsdle-util
)

target_include_directories(
        morse
        PUBLIC
        include
)

target_sources(
        morse
        PUBLIC
        morse.c
)

target_link_libraries(
        morse
        morsdle-util
)