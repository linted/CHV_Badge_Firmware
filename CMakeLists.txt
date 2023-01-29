# cmake_minimum_required(VERSION 3.20)
# include(deps/pico-sdk/pico_sdk_init.cmake) # Has to be here
# project(CHV_DC31 C CXX ASM)

# add_subdirectory(deps/micropython/ports/rp2)

set(INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/release)
file(MAKE_DIRECTORY ${INSTALL_DIR})

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/deps ${CMAKE_CURRENT_BINARY_DIR}/usermod_deps)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/verification ${CMAKE_CURRENT_BINARY_DIR}/usermod_verification)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/firmware ${CMAKE_CURRENT_BINARY_DIR}/usermod_firmware)
