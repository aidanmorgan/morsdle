add_library(morsdle-game)

add_dependencies(
        morsdle-game
        morsdle-util
)

target_include_directories(
        morsdle-game
        PUBLIC
        include
)

target_sources(
        morsdle-game
        PUBLIC
        morsdle.c
)

target_link_libraries(
        morsdle-game
        morsdle-util
)