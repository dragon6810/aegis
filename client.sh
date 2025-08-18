#!/usr/bin/env bash

mkdir -p build
cd build
if cmake .. -DEXEC_TARGET=launcher; then
    if cmake --build .; then
        cd ../run/launcher
        ../../build/bin/launcher
        exit 0
    fi
fi

exit 1