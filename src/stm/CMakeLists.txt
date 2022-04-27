cmake_minimum_required(VERSION 3.20)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/stm32_gcc.cmake)
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR}/cmake)

project(morsdle-stm C ASM)

set(HAL_COMP_LIST RCC GPIO CORTEX STM32L4)
set(CMSIS_COMP_LIST STM32L4)

find_package(
        CMSIS
        COMPONENTS "${CMSIS_COMP_LIST}"
        REQUIRED
        HINTS ${CMAKE_SOURCE_DIR}/lib/stm32l4xx/Drivers/CMSIS
)

find_package(HAL
        COMPONENTS "${HAL_COMP_LIST}"
        REQUIRED
        HINTS ${CMAKE_SOURCE_DIR}/lib/stm32l4xx/Drivers/STM32L4xx_HAL_Driver
)

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

target_link_libraries(
#        HAL::STM32::L0::RCC
 #       HAL::STM32::L0::GPIO
  #      HAL::STM32::L0::CORTEX
   #     CMSIS::STM32::L053C8
    #    STM32::NoSys

        morsdle-stm
        morsdle-util
        morsdle-gfx
        morsdle-game
        waveshare-display
)

target_sources(
        morsdle-stm
        PUBLIC
)

target_link_libraries(
        morsdle-stm
#        HAL::STM32::F4::LL_RCC
#        HAL::STM32::F4::LL_GPIO
#        CMSIS::STM32::F407VG
#        STM32::NoSys
)


