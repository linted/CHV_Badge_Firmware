#!/bin/bash

echo "This is only tested to work on ubuntu!!!"

wait_for_file () {
    echo "Waiting for $1"
    while [ 1 ]; do
        ls $1 1>/dev/null 2>&1 && break
        sleep 0.5
    done
    echo "found $1"
}

while [ 1 ]; do
    # flash with micropython
    echo "%%%% Connect badge and enter bootloader %%%%"
    echo "press reset + bootsel and release reset first"
    wait_for_file /media/$USER/RPI-RP2
    echo "Flashing firmware"
    cp firmware.uf2 /media/$USER/RPI-RP2/

    # flash with python code
    wait_for_file /dev/ttyACM0
    for files in *py usbd/ ; do
        echo "Flashing %files"
        ampy -p /dev/ttyACM0 put $files
    done

    # reset to make sure everything worked
    echo "##### PRESS RESET BUTTON #####"

done
