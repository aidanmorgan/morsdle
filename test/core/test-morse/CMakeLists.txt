add_executable(test-morse)

add_dependencies(
        test-morse
        unity
        morsdle-util
        morse
)

target_include_directories(
        test-morse
        PUBLIC
        include
)

target_sources(
        test-morse
        PUBLIC
        test_morse.c
)

target_link_libraries(
        test-morse
        unity
        morsdle-util
        morse
)

add_test(
        NAME test-morse
        COMMAND test-morse
)


