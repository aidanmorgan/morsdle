# morsdle
An implementation of Wordle, running on a STM32 where the input is a morse-key.


To build the STM code for a STM32 Nucleo-64 (STM32L476RG) specify the following command line arguments to cmake:

`-DSTM32_TOOLCHAIN_PATH=/home/aidan/gcc-arm -DCMAKE_TOOLCHAIN_FILE=$CMakeProjectDir$/cmake/stm32_gcc.cmake`

Dependencies:

ARM Development toolchain from: 
https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/downloads-1

Flashing:
make flash

Debugging:

* make gdbhost (to start the host)
* arm-none-eabi-gdb (in a new terminal window)
* target extended-remote localhost:3333 (to connect to the gdb host running in openocd)
* monitor reset halt (monitor prefix to issue openocd commands)
* file ~/dev/personal/morsdle/cmake-build-debug-arm/src/stm/main/morsdle-stm.elf
* monitor program /Users/aidan/dev/personal/morsdle/cmake-build-debug-arm/src/stm/main/morsdle-stm.elf 0x08000000 verify reset


arm-none-eabi-gdb ./src/stm/main/morsdle-stm.elf
target extended-remote localhost:3333
thbreak main



Pins:

From the Waveshare doco - STM32F103ZET
Vcc	- 3.3V
GND	- GND
DIN	- PA7  (MOSI1) - (AF_PP, FREQ_HIGH)
CLK	- PA5 (SCK1) - (AF_PP, FREQ_HIGH)
CS	- PA4 (NSS1?) - GPIOA-4  (OUTPUT_PP, NO_PULL, FREQ_LOW, RESET)
DC	- PA2 (GPIO) - GPIOA-2   (OUTPUT_PP, NO_PULL, FREQ_LOW, RESET)
RST	- PA1 (GPIO) - GPIOA-1   (OUTPUT_PP, NO_PULL, FREQ_LOW, RESET)
BUSY - PA3 (GPIO) - GPIOA-3  (INPUT, NO_PULL)

SPI Settings:
MASTER, 2LINE, 8BIT, POLARITY_LOW, PHASE_1EDGE, NSS_SOFT, BAUDRATEPRESCALER_64, FIRSTBIT_MSB, TIMODE_DISABLE, CRCCALCULATION_DISABLE, CRCPolynomial=10




