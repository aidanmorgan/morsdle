add_executable(test-util)

add_dependencies(
        test-util
        morsdle-util
        unity
)

target_include_directories(
        test-util
        PUBLIC
        include
)

target_sources(
        test-util
        PUBLIC
        test_cbuff.c
)

target_link_libraries(
        test-util
        unity
        morsdle-util
)

add_test(
        NAME test-util
        COMMAND test-util
)
