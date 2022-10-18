
add_executable(
    firmware 
    firmware.c
)

target_link_libraries(
    firmware PUBLIC pico_stdlib can2040
)
