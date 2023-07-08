
add_executable(
    verify
    verify.c
)

target_link_libraries(verify PUBLIC 
    pico_stdlib
    pico_stdio_usb
    pico_printf
    pico_multicore
)


pico_add_uf2_output(verify)
# example_auto_set_url(verify)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/$<IF:$<BOOL:$<TARGET_PROPERTY:verify,OUTPUT_NAME>>,$<TARGET_PROPERTY:verify,OUTPUT_NAME>,$<TARGET_PROPERTY:verify,NAME>>.uf2
    DESTINATION ${CMAKE_INSTALL_PREFIX}
)