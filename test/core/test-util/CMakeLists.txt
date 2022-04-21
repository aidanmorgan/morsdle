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
        test-llist.c
)

target_link_libraries(
        test-util
        unity
        morsdle-util
)

