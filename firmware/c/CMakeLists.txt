
function(firmware TARGET)
    add_executable(
        ${TARGET}
        ${ARGN}
    )
    target_link_libraries(
        ${TARGET} PRIVATE 
        pico_stdlib
        pico_multicore
        pico_stdio_usb
        pico_printf
        can2040
    )
    pico_add_uf2_output(${TARGET})
    # the uf2 output function doesn't easily expose where the file ends up, so copy their output file name gen code
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/$<IF:$<BOOL:$<TARGET_PROPERTY:${TARGET},OUTPUT_NAME>>,$<TARGET_PROPERTY:${TARGET},OUTPUT_NAME>,$<TARGET_PROPERTY:${TARGET},NAME>>.uf2
        DESTINATION ${INSTALL_DIR}
    )
endfunction()

# firmware(
#     rp1_firmware 
#     rp1_firmware.c
# )

# firmware(
#     rp2_firmware 
#     rp2_firmware.c
# )

firmware(
    rp3_firmware 
    rp3_firmware.c
)