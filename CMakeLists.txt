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
    add_subdirectory(src/drivers/waveshare)
    add_subdirectory(src/stm)
endif()

#optionally build unit tests
add_subdirectory(test/core)