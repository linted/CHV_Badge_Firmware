
########### rp2040 init ###########
pico_sdk_init()

########### CAN2040 stuff ###########
add_library(
    can2040 STATIC
    can2040/src/can2040.c
)
target_include_directories(can2040 PUBLIC can2040/src/)
target_link_libraries(can2040 PUBLIC pico_stdlib cmsis_core)

########### MicroPython stuff ###########
set(MP_DIR "${CMAKE_CURRENT_LIST_DIR}/micropython/mpy-cross" CACHE STRING "" )
set(MPY_CROSS ${MP_DIR}/build/mpy-cross CACHE STRING "")
set(MPY_TOOL ${MP_DIR}/../tools/mpy-tool.py CACHE STRING "")
set(MPY_LD ${MP_DIR}/../tools/mpy_ld.py CACHE STRING "")


ExternalProject_Add( mpy_cross
    SOURCE_DIR ${MP_DIR}
    BINARY_DIR ${MP_DIR}
    # No download step
    # No update
    # No patch
    CONFIGURE_COMMAND "" # No configure needed, but must be specified
    # default build command
    INSTALL_COMMAND "" # Don't actually install
    # No test
)

function(add_mpy_py TARGET FILE)
    set(output_file ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}.mpy)
    add_custom_command(
        OUTPUT ${output_file}
        COMMAND ${MPY_CROSS} ${FILE} -o ${output_file}
        DEPENDS mpy_cross
    )

    add_custom_target(
        ${TARGET} ALL 
        DEPENDS ${output_file}
    )
    install(FILES ${output_file} DESTINATION ${INSTALL_DIR})
endfunction()


function(add_mpy_c TARGET)


    # preprocess all source files
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_config.h
        COMMAND ${MPY_LD} --arch armv7 --preprocess -o ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_config.h ${ARGN}
    )
    set(TARGET_O_FILES) # declare it here
    # build .o from .c
    foreach(file in ${ARGN})
        add_custom_command(
            OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${file}.o
            COMMAND ${CMAKE_C_COMPILER} -mthumb -mcpu=cortex-m0plus ${CMAKE_C_FLAGS_${CMAKE_BUILD_TYPE}} -I${MP_DIR}/../ -I${MP_DIR}/../ports/rp2/ -o ${CMAKE_CURRENT_BINARY_DIR}/${file}.o ${file}
        )
        list(APPEND TARGET_O_FILES ${CMAKE_CURRENT_BINARY_DIR}/${file}.o)
    endforeach()

    # link
    add_custom_command(
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_config.h ${TARGET_O_FILES}
        COMMAND ${MPY_LD} --arch armv7 --qstrs ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_config.h -o ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}.native.mpy ${TARGET_O_FILES}
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}.native.mpy
    )

    add_custom_target(
        ${TARGET} ALL
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}.native.mpy
    )
endfunction()