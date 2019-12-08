#!/bin/bash

# update submodule
git submodule init
git submodule update

if [ ! -d build ]; then
    mkdir build
    cd build
else
    cd build
fi

cmake ..
make -j 8

echo "build complete"
