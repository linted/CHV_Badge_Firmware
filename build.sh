#!/bin/bash

MISSING_DEPS=0
if [ ! $(which cmake) ]; then
  echo "Missing cmake!"
  MISSING_DEPS=1
fi
if [ ! $(which arm-none-eabi-gcc) ]; then
  echo "missing arm gcc!"
  MISSING_DEPS=1
fi

if [ ! $MISSING_DEPS ]; then
  exit 1
fi

pushd $(dirname -- "${BASH_SOURCE[0]}")
  mkdir -p build
  pushd build
    cmake ..
    make -j4
  popd
popd
