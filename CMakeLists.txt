cmake_minimum_required(VERSION 3.20)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

project(morsdle)

enable_testing()

# build the core libraries
add_subdirectory(src/core)
add_subdirectory(src/drivers/waveshare-util)
add_subdirectory(src/drivers/waveshare-display)
add_subdirectory(src/drivers/waveshare-api)


if(CMAKE_SYSTEM_PROCESSOR STREQUAL arm)
    set(STM32_LOCAL_CMSIS_LIB_PATH ${CMAKE_SOURCE_DIR}/lib/stm32l4xx/Drivers/CMSIS)
    set(STM32_LOCAL_HAL_LIB_PATH ${CMAKE_SOURCE_DIR}/lib/stm32l4xx/Drivers/STM32L4xx_HAL_Driver)

    add_subdirectory(src/stm/main)
else()
    add_subdirectory(lib/unity)
    add_subdirectory(lib/svg)

    add_subdirectory(test/core)
endif()

