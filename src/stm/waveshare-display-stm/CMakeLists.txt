cmake_minimum_required(VERSION 3.20)

find_package(CMSIS COMPONENTS STM32L4 REQUIRED)
find_package(HAL COMPONENTS STM32L4 RCC GPIO CORTEX SPI REQUIRED)

project(waveshare-display-stm C ASM)

add_library(waveshare-display-stm)

add_dependencies(
        waveshare-display-stm

        morsdle-stm-shared
        morsdle-gfx
)

target_include_directories(
        waveshare-display-stm
        PUBLIC
        include
        HAL::STM32::L4::RCC
        HAL::STM32::L4::SPI
        HAL::STM32::L4::GPIO
        HAL::STM32::L4::CORTEX
        CMSIS::STM32::L4
)


target_sources(
        waveshare-display-stm

        PUBLIC
        waveshare_stm.c
)

target_link_libraries(
        waveshare-display-stm

        morsdle-gfx
        morsdle-stm-shared
        HAL::STM32::L4::RCC
        HAL::STM32::L4::SPI
        HAL::STM32::L4::GPIO
        HAL::STM32::L4::CORTEX
        CMSIS::STM32::L4
)
