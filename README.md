# CHV Car on a board
This project is intended to provide firmware for emulating a car on a single PCB.

# Layout
```
/
└─── verification/
└─── firmware/
└─── tests/
└─── deps/
```
## Verification
Files in this folder are for use by manufacturer to verify that all electrical connections are correct.

## Firmware
Firmware folder contains the firmware which actually emulates the car

## Tests
This should contain any unit tests or similar items.

## Deps
Any source code dependencies needed to build the project

# Requirments
You will need the following packages installed
```
gcc-arm-none-eabi
cmake
```

## elftools
install elftools with:
```
pip install 'pyelftools>=0.25'
```



# Building

_*NOTE:*_ Run the following command to fetch git the dependencies.
```
git submodule update --init
```

To build run:
```
./build.sh
```

You can rebuild cleanly by running:
```
./build.sh clean
```

# cansniffer
rp3 is set up to log output in a format readable by socketcan.
Run the following commands to get everything setup.

```
sudo slcand /dev/ttyACM0
sudo ip link set slcan0 up
cansniffer slcan0
```
