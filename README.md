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

_*NOTE:*_ Run the following command to fetch the dependencies.
```
git submodule update --init --recursive
```