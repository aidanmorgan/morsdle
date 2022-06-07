cmake_minimum_required(VERSION 3.20)

add_library(waveshare-util)

target_include_directories(
        waveshare-util
        PUBLIC
        include
)

target_sources(
        waveshare-util
        PUBLIC
        imagebuffer.c
)

