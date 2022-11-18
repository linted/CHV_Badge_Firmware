#!/bin/bash

if [[ "${1,,}" == "clean" ]]; then 
  rm -rf build/ release/
fi

MISSING_DEPS=0
if [ ! $(which cmake) ]; then
  echo "Missing cmake!"
  MISSING_DEPS=1
fi
if [ ! $(which arm-none-eabi-gcc) ]; then
  echo "missing arm gcc!"
  MISSING_DEPS=1
fi
if [ $(python3 -c "from elftools.elf import elffile") ]; then
  echo "missing elftools!"
  MISSING_DEPS=1
fi

if [ $MISSING_DEPS -eq 1 ]; then
  exit 1
fi

pushd deps/micropython/ports/rp2
  make submodules
popd

pushd $(dirname -- "${BASH_SOURCE[0]}")
  mkdir -p build
  pushd build
    # cmake .. -DCMAKE_INSTALL_PREFIX=$(pwd)/../release
    cmake ../deps/micropython/ports/rp2 -DUSER_C_MODULES=$(pwd)/../CMakeLists.txt
    cmake --build . -j4
  popd
popd
