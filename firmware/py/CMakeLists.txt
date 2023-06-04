add_mpy_py(boot ${CMAKE_CURRENT_SOURCE_DIR}/boot.py)
add_mpy_py(main ${CMAKE_CURRENT_SOURCE_DIR}/main.py)
add_mpy_py(slcan ${CMAKE_CURRENT_SOURCE_DIR}/slcan.py)
add_mpy_py(leds ${CMAKE_CURRENT_SOURCE_DIR}/leds.py)
add_mpy_py(can ${CMAKE_CURRENT_SOURCE_DIR}/can.py)

# I'm tired of talking about why this is commented out
# add_subdirectory(can2040_py)


install(FILES ${CMAKE_CURRENT_LIST_DIR}/install.sh DESTINATION ${CMAKE_INSTALL_PREFIX})