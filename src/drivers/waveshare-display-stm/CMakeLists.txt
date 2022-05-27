cmake_minimum_required(VERSION 3.20)

add_library(waveshare-stm)

find_package(HAL COMPONENTS STM32L4 LL_GPIO REQUIRED)

add_dependencies(
        waveshare-stm
        morsdle-gfx
)

target_include_directories(
        waveshare-stm
        PUBLIC
        include
)


target_sources(
        waveshare-stm
        PUBLIC
        waveshare_stm.c
)

target_link_libraries(
        waveshare-stm
        morsdle-gfx
        HAL::STM32::L4::LL_GPIO
)
