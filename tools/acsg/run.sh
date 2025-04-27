#!/usr/bin/env bash

if make; then
    mkdir -p run
    cd run
    ../bin/acsg zombieattack
fi