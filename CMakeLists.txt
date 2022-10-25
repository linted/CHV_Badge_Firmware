cmake_minimum_required(VERSION 3.20)
include(deps/pico-sdk/pico_sdk_init.cmake) # Has to be here
project(CHV_DC31 C CXX ASM)

add_subdirectory(deps)
add_subdirectory(verification)
add_subdirectory(firmware)