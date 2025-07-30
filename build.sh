#!/usr/bin/env bash

mkdir -p build
cd build
if cmake .. -DEXEC_TARGET=ALL; then
    if cmake --build .; then
        exit 0
    fi
fi

exit 1