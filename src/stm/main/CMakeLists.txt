cmake_minimum_required(VERSION 3.20)


project(morsdle-stm C ASM)
add_executable(morsdle-stm)

target_compile_definitions(morsdle-stm PUBLIC STM32L476xx)

find_package(CMSIS COMPONENTS STM32L4 REQUIRED)
find_package(HAL COMPONENTS STM32L4 REQUIRED)


add_dependencies(
        morsdle-stm

        morsdle-util
        morsdle-gfx
        morsdle-game
        morse
        waveshare-api
        waveshare-display
)

target_include_directories(
        morsdle-stm
        PUBLIC
        include
        ${CMAKE_SOURCE_DIR}/ioc/morsdle/core/Inc
)

# the glob rule here is weird, but it seems CMake drops the last term when doing this
file(GLOB STM32CUBEMX_GENERATED_SRC  ${CMAKE_SOURCE_DIR}/ioc/morsdle/Core/*/*.c)
message("STM32CUBEMX_GENERATED_SRC: ${STM32CUBEMX_GENERATED_SRC}")

target_sources(
        morsdle-stm
        PUBLIC
        morsdle_main.c
        ${STM32CUBEMX_GENERATED_SRC}
)

target_link_libraries(
        morsdle-stm

        morsdle-util
        morsdle-gfx
        morsdle-game
        morse
        waveshare-api
        waveshare-display

        HAL::STM32::L4
        CMSIS::STM32::L4
        STM32::NoSys
)

stm32_print_size_of_target(morsdle-stm)


