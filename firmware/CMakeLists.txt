
add_executable(
    rp1_firmware 
    rp1_firmware.c
)
target_link_libraries(
    rp1_firmware PUBLIC pico_stdlib can2040
)
pico_add_extra_outputs(rp1_firmware)



add_executable(
    rp2_firmware 
    rp2_firmware.c
)
target_link_libraries(
    rp2_firmware PUBLIC pico_stdlib can2040
)
pico_add_extra_outputs(rp2_firmware)



add_executable(
    rp3_firmware 
    rp3_firmware.c
)
target_link_libraries(
    rp3_firmware PUBLIC pico_stdlib pico_multicore pico_stdio_usb pico_printf can2040
)
pico_add_extra_outputs(rp3_firmware)