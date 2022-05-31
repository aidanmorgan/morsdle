cmake_minimum_required(VERSION 3.20)

find_package(HAL COMPONENTS STM32L4 LL_GPIO REQUIRED)


project(waveshare-display-stm C ASM)

add_library(waveshare-display-stm)

add_dependencies(
        waveshare-display-stm
        morsdle-gfx
)

target_include_directories(
        waveshare-display-stm
        PUBLIC
        include
)


target_sources(
        waveshare-display-stm
        PUBLIC
        waveshare_stm.c
)

target_link_libraries(
        waveshare-display-stm
        morsdle-gfx
)
