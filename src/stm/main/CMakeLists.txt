cmake_minimum_required(VERSION 3.20)

find_package(CMSIS COMPONENTS STM32L4 REQUIRED)
find_package(HAL COMPONENTS STM32L4 REQUIRED)

project(morsdle-stm C ASM)
add_executable(morsdle-stm)

add_dependencies(
        morsdle-stm

        morsdle-stm32l4
        morsdle-util
        morsdle-gfx
        morsdle-game
        waveshare-api
        waveshare-display
        morse
)

target_include_directories(
        morsdle-stm
        PUBLIC
        include
)

target_sources(
        morsdle-stm
        PUBLIC
        morsdle_main.c
)

target_link_libraries(
        morsdle-stm

        morsdle-stm32l4
        morsdle-util
        morsdle-gfx
        morsdle-game
        waveshare-api
        waveshare-display
        morse

        HAL::STM32::L4::RCC
        HAL::STM32::L4::GPIO
        HAL::STM32::L4::SPI
        HAL::STM32::L4::CORTEX
        #CMSIS::STM32::L476RG
        STM32::NoSys
)

stm32_print_size_of_target(morsdle-stm)


