#!/usr/bin/env bash

mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXEC_TARGET=All ..