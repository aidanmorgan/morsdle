add_executable(test-cbuff)
add_executable(test-imagebuffer)

add_dependencies(
        test-cbuff
        morsdle-util
        unity
)
add_dependencies(
        test-imagebuffer
        morsdle-util
        unity
)

target_sources(
        test-cbuff
        PUBLIC
        test_cbuff.c
)

target_sources(
        test-imagebuffer
        PUBLIC
        test_imagebuffer.c
)

target_link_libraries(
        test-cbuff
        unity
        morsdle-util
)
target_link_libraries(
        test-imagebuffer
        unity
        morsdle-util
)

add_test(
        NAME test-cbuff
        COMMAND test-cbuff
)
add_test(
        NAME test-imagebuffer
        COMMAND test-imagebuffer
)
