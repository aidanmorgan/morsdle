add_library(morsdle-stm32l4)

find_package(CMSIS COMPONENTS STM32L4 REQUIRED)
find_package(HAL COMPONENTS STM32L4 REQUIRED)

target_include_directories(
        morsdle-stm32l4
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/morsdle/Core/Inc
)

target_sources(
        morsdle-stm32l4
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/morsdle/Core/Src/main.c
        ${CMAKE_CURRENT_LIST_DIR}/morsdle/Core/Src/stm32l4xx_hal_msp.c
        ${CMAKE_CURRENT_LIST_DIR}/morsdle/Core/Src/stm32l4xx_it.c
        ${CMAKE_CURRENT_LIST_DIR}/morsdle/Core/Src/system_stm32l4xx.c
)

target_link_libraries(
        morsdle-stm32l4
        HAL::STM32::L4
        CMSIS::STM32::L4
)