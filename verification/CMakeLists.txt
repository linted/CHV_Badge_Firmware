
add_executable(
    verify
    verify.c
)

target_link_libraries(verify PUBLIC pico_stdlib)
pico_add_extra_outputs(verify)
# example_auto_set_url(verify)

add_mpy_py(verify_code ${CMAKE_CURRENT_SOURCE_DIR}/code.py)