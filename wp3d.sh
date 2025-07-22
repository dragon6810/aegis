#!/usr/bin/env bash

if ./build.sh; then
    cd run/wp3d
    ../../build/bin/wp3d
    exit 0
fi

exit 1