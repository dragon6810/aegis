#!/usr/bin/env bash

mkdir -p build
cd build
if cmake .. -DEXEC_TARGET=tests; then
    if cmake --build .; then
        cd ../run/tests
        ../../build/bin/tests
        exit 0
    fi
fi

exit 1