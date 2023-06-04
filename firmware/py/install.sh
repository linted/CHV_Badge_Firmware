#!/bin/bash

echo "This is only tested to work on ubuntu!!!"

RES=$(inotifywait -e create /media/$USER/ --format %f .)

cp firmware.uf2 /media/$USER/$RES/

while tty=$(inotifywait -e create /dev --format %f .); do
    if [[ $tty =~ 'tty.*' ]]; then
        for files in *.py *.mpy ; do
            ampy -p /dev/$tty put $file
        done
        break
    fi
done
