cmake_minimum_required(VERSION 3.20)

find_package(CMSIS COMPONENTS STM32L4 REQUIRED)
find_package(HAL COMPONENTS STM32L4 LL_RCC LL_GPIO REQUIRED)


project(morsdle-stm C ASM)
add_executable(morsdle-stm)

add_dependencies(
        morsdle-stm
        morsdle-util
        morsdle-gfx
        morsdle-game
        waveshare-display-stm
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
        main.c
)

target_link_libraries(
        morsdle-stm
        morsdle-util
        morsdle-gfx
        morsdle-game
        waveshare-stm
        waveshare-display
        waveshare-display-stm
        morse

        HAL::STM32::L4::LL_RCC
        HAL::STM32::L4::LL_GPIO
        CMSIS::STM32::L476RG
        STM32::NoSys
)

stm32_print_size_of_target(morsdle-stm)


