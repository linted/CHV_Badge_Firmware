
add_executable(
    rp1_firmware 
    rp1_firmware.c
)

target_link_libraries(
    rp1_firmware PUBLIC pico_stdlib can2040
)
pico_add_extra_outputs(rp1_firmware)
