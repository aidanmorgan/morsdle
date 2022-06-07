add_library(morsdle-util)

target_include_directories(
        morsdle-util
        PUBLIC
        include
)

target_sources(
        morsdle-util
        PUBLIC
        cbuff.c
        imagebuffer.c
)
