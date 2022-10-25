#!/bin/bash

pushd $(dirname -- "${BASH_SOURCE[0]}")

  mkdir -p build
  pushd build
    cmake ..
    make -j4
  popd

popd
