cmake_minimum_required(VERSION 3.20)

project(waveshare-api C)
add_library(waveshare-api)

add_dependencies(
        waveshare-api
        morsdle-gfx
        morsdle-util
)

target_include_directories(
        waveshare-api
        PUBLIC
        include
)

target_sources(
        waveshare-api
        PUBLIC
        waveshare_api.c
)

target_link_libraries(
        waveshare-api
        morsdle-gfx
        morsdle-util
)
