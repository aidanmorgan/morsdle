cmake_minimum_required(VERSION 3.20)


project(morsdle-stm C ASM)

add_executable(morsdle-stm)

set(PRJ morsdle-stm)

target_compile_definitions(morsdle-stm PUBLIC STM32L476xx DEBUG)

find_package(CMSIS COMPONENTS STM32L476 REQUIRED)
find_package(HAL COMPONENTS STM32L476 REQUIRED)

file(GLOB STM32CUBEMX_GENERATED_SRC ${CMAKE_SOURCE_DIR}/ioc/morsdle/Core/*/main.c ${CMAKE_SOURCE_DIR}/ioc/morsdle/Core/*/stm32l4*.c)
message("STM32CUBEMX_GENERATED_SRC: ${STM32CUBEMX_GENERATED_SRC}")

#target_compile_options(morsdle-stm PRIVATE -g)


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

target_sources(
        morsdle-stm
        PUBLIC
        morsdle_main.c
        game.c
        loader.c
        debug.c
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

        HAL::STM32::L4::CORTEX
        HAL::STM32::L4::DMA
        HAL::STM32::L4::FLASH
        HAL::STM32::L4::GPIO
        HAL::STM32::L4::I2C
        HAL::STM32::L4::SPI
        HAL::STM32::L4::PWR
        HAL::STM32::L4::PWREx
        HAL::STM32::L4::RCC
        HAL::STM32::L4::RCCEx
        HAL::STM32::L4::TIM
        HAL::STM32::L4::TIMEx
        HAL::STM32::L4::UART
        HAL::STM32::L4::RNG
        HAL::STM32::L4::LL_RNG
        HAL::STM32::L4::QSPI
        CMSIS::STM32::L476RG
        STM32::NoSys
)

stm32_print_size_of_target(morsdle-stm)
stm32_generate_binary_file(morsdle-stm)
stm32_generate_hex_file(morsdle-stm)

# Flash firmware to target.
add_custom_target(flash
        openocd
        -f interface/stlink.cfg
        -f board/stm32l4discovery.cfg
        -c "init"
        -c "reset halt"
        -c "flash write_image erase ${PRJ}.elf"
        -c "flash verify_image ${PRJ}.elf"
        -c "reset run"
        -c "shutdown"
        DEPENDS ${PRJ}.elf
        COMMENT "Flashing target hardware")


# Run OpenOCD as a GDB host.
add_custom_target(gdb
        openocd
        -f interface/stlink.cfg
        -f board/stm32l4discovery.cfg
        -c "init"
        -c "reset halt"
        DEPENDS ${PRJ}.elf
        COMMENT "Running OpenOCD as a GDB host.")


