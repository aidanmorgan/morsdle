add_executable(test-core)

add_dependencies(
        test-core
        morsdle-game
        morsdle-util
        unity
)

target_include_directories(
        test-core
        PUBLIC
        include
)

target_sources(
        test-core
        PUBLIC
        test_morsdle.c
)

target_link_libraries(
        test-core
        unity
        morsdle-game
)

