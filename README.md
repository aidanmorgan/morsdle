# morsdle
An implementation of Wordle, running on a STM32 where the input is a morse-key.


To build the STM code for a STM32 Nucleo-64 (STM32L476RG) specify the following command line arguments to cmake:

`-DSTM32_TOOLCHAIN_PATH=/home/aidan/gcc-arm -DCMAKE_TOOLCHAIN_FILE=$CMakeProjectDir$/cmake/stm32_gcc.cmake`