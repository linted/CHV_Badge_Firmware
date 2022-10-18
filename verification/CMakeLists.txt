
add_executable(
    verify
    verify.c
)

target_link_libraries(verify PUBLIC pico_stdlib)
pico_add_extra_outputs(blink)
example_auto_set_url(blink)