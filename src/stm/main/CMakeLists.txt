cmake_minimum_required(VERSION 3.20)


project(morsdle-stm C ASM)
add_executable(morsdle-stm)

target_compile_definitions(morsdle-stm PUBLIC STM32L476xx)

set(CMSIS_DIR ${CMAKE_SOURCE_DIR}/lib/stm32l4xx/Drivers/CMSIS)
set(DEVICE_DIR ${CMAKE_SOURCE_DIR}/lib/stm32l4xx/Drivers/CMSIS/Device/ST/STM32L4xx)
set(HAL_DIR ${CMAKE_SOURCE_DIR}/lib/stm32l4xx/Drivers/STM32L4xx_HAL_Driver)
set(IOC_DIR ${CMAKE_SOURCE_DIR}/ioc/morsdle)

file(GLOB STM32CUBEMX_GENERATED_SRC  ${IOC_DIR}/Core/*/*.c)
message("STM32CUBEMX_GENERATED_SRC: ${STM32CUBEMX_GENERATED_SRC}")



set(CMAKE_C_COMPILER "arm-none-eabi-gcc")

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
        ${CMSIS_DIR}/Include
        ${DEVICE_DIR}/Include
        ${HAL_DIR}/Inc
)

set_property(SOURCE ${IOC_DIR}/startup_stm32l476xx.s PROPERTY LANGUAGE ASM)

target_compile_options(morsdle-stm PUBLIC -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard)

target_sources(
        morsdle-stm
        PUBLIC
        morsdle_main.c
        ${STM32CUBEMX_GENERATED_SRC}
#        ${DEVICE_DIR}/Source/Templates/system_stm32l4xx.c
        ${IOC_DIR}/startup_stm32l476xx.s
        ${HAL_DIR}/Src/stm32l4xx_hal.c
        ${HAL_DIR}/Src/stm32l4xx_hal_cortex.c
        ${HAL_DIR}/Src/stm32l4xx_hal_dma.c
        ${HAL_DIR}/Src/stm32l4xx_hal_flash.c
        ${HAL_DIR}/Src/stm32l4xx_hal_flash_ex.c
        ${HAL_DIR}/Src/stm32l4xx_hal_gpio.c
        ${HAL_DIR}/Src/stm32l4xx_hal_i2c.c
        ${HAL_DIR}/Src/stm32l4xx_hal_i2c_ex.c
        ${HAL_DIR}/Src/stm32l4xx_hal_spi.c
        ${HAL_DIR}/Src/stm32l4xx_hal_spi_ex.c
        ${HAL_DIR}/Src/stm32l4xx_hal_pwr.c
        ${HAL_DIR}/Src/stm32l4xx_hal_pwr_ex.c
        ${HAL_DIR}/Src/stm32l4xx_hal_rcc.c
        ${HAL_DIR}/Src/stm32l4xx_hal_rcc_ex.c
        ${HAL_DIR}/Src/stm32l4xx_hal_tim.c
        ${HAL_DIR}/Src/stm32l4xx_hal_tim_ex.c
        ${HAL_DIR}/Src/stm32l4xx_hal_uart.c
        ${HAL_DIR}/Src/stm32l4xx_hal_uart_ex.c
)


target_link_options(morsdle-stm PUBLIC -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -T${IOC_DIR}/STM32L476RGTx_FLASH.ld -Wl,--gc-section)


target_link_libraries(
        morsdle-stm

        morsdle-util
        morsdle-gfx
        morsdle-game
        morse
        waveshare-api
        waveshare-display
)

stm32_print_size_of_target(morsdle-stm)

add_custom_command(
        TARGET morsdle-stm
        COMMAND "arm-none-eabi-objdump"
        ARGS "-S" "morsdle-stm.elf" ">>" "morsdle-stm.lst")

# Flash firmware to target.
add_custom_target(flash
        openocd -f interface/stlink-v2-1.cfg -c "transport select hla_swd"
        -f target/stm32l4x.cfg -c "init" -c "reset halt"
        -c "flash write_image erase morsdle-stm.elf" -c "reset run" -c "shutdown"
        DEPENDS morsdle-stm.elf
        COMMENT "Flashing target hardware")

# Run OpenOCD as a GDB host.
add_custom_target(gdbhost
        openocd -f interface/stlink-v2-1.cfg -c "transport select hla_swd"
        -f target/stm32l4x.cfg -c "init" -c "reset halt"
        DEPENDS morsdle-stm}.elf
        COMMENT "Running OpenOCD as a GDB host.")


