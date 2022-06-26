# morsdle
An implementation of Wordle, running on a STM32 where the input is a morse-key.

![Working system](/doc/assembled.jpeg "Assembled Solution")


To build the STM code for a STM32 Nucleo-64 (STM32L476RG) specify the following command line arguments to cmake:

`-DSTM32_TOOLCHAIN_PATH=/home/aidan/gcc-arm -DCMAKE_TOOLCHAIN_FILE=$CMakeProjectDir$/cmake/stm32_gcc.cmake`

# Dependencies:

## ARM Development Toolchain:

Link: https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/downloads-1

You'll need to compile this for your platform (I am using a mac and it works fine)

## OpenOCD

Used by the cmake scripts to flash and debug (using gdb), you'll need to compile this for your platform yourself

## Other
There are copies of some libraries as submodules in the lib directory, make sure to init those (I don't use the HAL or CMSIS from CubeMX, but use the submodule version).

# Parts:

## Core
* STM32L476RG (NUCLEO-L476RG)
* Waveshare 5.56" 7-colour e-ink display
* 32 Mb NOR Flash (QSpi)

## Hardware Debounce:
* 74HC14 (Inverting Schmitt Trigger)
* 0.1 uF Cap
* 10k resistors (2)
* SPDT (long lever) switch

# Pinout:

Can be changed by opening the .ioc file in the ioc directory and re-generating the code, but this is what I have configured:

## To Waveshare Display:
* Vcc	- 3.3V  (light purple)
* GND	- GND  (brown/maroon)
* DIN	-  (MOSI2)  (dark blue)
* CLK	- (SCK2)  (yellow)
* CS	- (GPIOC9) (orange)
* DC	- (GPIOC7)  (green)
* RST	- (GPIOC8)  (white)
* BUSY - (GPIOC6)  (dark purple)

## "game mode" and "loader mode":
* Add 3.3v to GPIOC1 and reset the board, will then accept commands via UART to write words and fonts into the flash memory


