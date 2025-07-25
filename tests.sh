#!/usr/bin/env bash

if ./build.sh; then
    cd run/tests
    ../../build/bin/tests
    exit $?
fi

exit 1