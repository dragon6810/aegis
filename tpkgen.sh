#!/usr/bin/env bash

mkdir -p build
cd build
if cmake .. -DCMAKE_BUILD_TYPE=Debug -DEXEC_TARGET=tpkgen; then
    if cmake --build .; then
        cd ../run/tpkgen
        ../../build/bin/tpkgen maketex.ascript
        exit 0
    fi
fi

exit 1