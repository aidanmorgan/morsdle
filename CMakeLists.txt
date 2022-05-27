cmake_minimum_required(VERSION 3.20)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

project(morsdle)

enable_testing()

# build this first before the remainder
add_subdirectory(lib/unity)
add_subdirectory(lib/svg)

# build the core libraries
add_subdirectory(src/core)

if(CMAKE_SYSTEM_PROCESSOR STREQUAL arm)
    set(STM32_LOCAL_CMSIS_LIB_PATH ${CMAKE_SOURCE_DIR}/lib/stm32l4xx/Drivers/CMSIS)
    set(STM32_LOCAL_HAL_LIB_PATH ${CMAKE_SOURCE_DIR}/lib/stm32l4xx/Drivers/STM32L4xx_HAL_Driver)

    add_subdirectory(src/drivers/waveshare-display-stm)
    add_subdirectory(src/drivers/waveshare)
    add_subdirectory(src/stm)
else()
    add_subdirectory(test/core)
endif()

#optionally build unit tests
