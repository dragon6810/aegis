#!/usr/bin/env bash

mkdir -p build
cd build
if cmake .. -DEXEC_TARGET=engine; then
    if cmake --build .; then
        cd ../run/engine
        ../../build/bin/engine
        exit 0
    fi
fi

exit 1