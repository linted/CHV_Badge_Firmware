
pico_sdk_init()

add_library(
    can2040 STATIC
    can2040/src/can2040.c
)
target_include_directories(can2040 PUBLIC can2040/src/)
target_link_libraries(can2040 PUBLIC pico_stdlib cmsis_core)