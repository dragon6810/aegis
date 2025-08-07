#!/usr/bin/env bash

mkdir -p build
cd build
if cmake .. -DEXEC_TARGET=bspgen; then
    if cmake --build .; then
        mkdir -p ../run/bspgen
        cd ../run/bspgen
        ../../build/bin/bspgen
        exit 0
    fi
fi

exit 1