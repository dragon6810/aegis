#!/usr/bin/env bash

if ./build.sh; then
    cd run/engine
    ../../build/bin/engine
    exit 0
fi

exit 1