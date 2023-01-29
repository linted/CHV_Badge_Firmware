
add_library(slcan STATIC
    slcan_output.c
)
target_include_directories(slcan PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(slcan PUBLIC pico_printf pico_stdlib can2040)