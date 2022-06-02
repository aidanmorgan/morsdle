cmake_minimum_required(VERSION 3.20)

find_package(CMSIS COMPONENTS STM32L4 REQUIRED)
find_package(HAL COMPONENTS STM32L4 REQUIRED)

add_library(morsdle-stm-shared INTERFACE)


target_include_directories(
        morsdle-stm-shared
        INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        HAL::STM32::L4
)
