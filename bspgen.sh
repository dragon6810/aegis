#!/usr/bin/env bash

mkdir -p build
cd build
if cmake .. -DEXEC_TARGET=bspgen; then
    if cmake --build .; then
        mkdir -p ../run/bspgen
        cd ../run/bspgen
        ../../build/bin/bspgen -v --csg-output csgout --hull-definition hulls bspgen
    fi
fi