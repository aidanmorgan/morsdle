cmake_minimum_required(VERSION 3.20)

set(STM32_LOCAL_CMSIS_LIB_PATH ${CMAKE_SOURCE_DIR}/lib/stm32l4xx/Drivers/CMSIS)
set(STM32_LOCAL_HAL_LIB_PATH ${CMAKE_SOURCE_DIR}/lib/stm32l4xx/Drivers/STM32L4xx_HAL_Driver)

project(morsdle-stm C ASM)

find_package(CMSIS COMPONENTS STM32L4 REQUIRED)
find_package(HAL COMPONENTS STM32L4 LL_RCC LL_GPIO REQUIRED)

add_executable(morsdle-stm)

add_dependencies(
        morsdle-stm
        morsdle-util
        morsdle-gfx
        morsdle-game
        waveshare-display
)

target_include_directories(
        morsdle-stm
        PUBLIC
        include
)


target_sources(
        morsdle-stm
        PUBLIC
        main.c
)

target_link_libraries(
        morsdle-stm
        morsdle-util
        morsdle-gfx
        morsdle-game
        waveshare-display

        HAL::STM32::L4::LL_RCC
        HAL::STM32::L4::LL_GPIO
        CMSIS::STM32::L476RG
        STM32::NoSys
)

stm32_print_size_of_target(morsdle-stm)


