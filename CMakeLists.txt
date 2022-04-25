cmake_minimum_required(VERSION 3.20)

project(morsdle)

# build this first before the remainder
add_subdirectory(lib/unity)
add_subdirectory(lib/svg)

# build the core libraries
add_subdirectory(src/core)
add_subdirectory(src/drivers/waveshare)
add_subdirectory(src/stm)

#optionally build unit tests
add_subdirectory(test/core)