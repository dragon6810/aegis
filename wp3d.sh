#!/usr/bin/env bash

mkdir -p build
cd build
if cmake .. -DEXEC_TARGET=wp3d; then
    if cmake --build .; then
        cd ../run/wp3d
        ../../build/bin/wp3d
        exit 0
    fi
fi

exit 1