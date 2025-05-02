#!/usr/bin/env bash

if make; then
    mkdir -p run
    cd run
    ../bin/acsg zombieattack
    python3 ../gtoobj.py zombieattack.g0 zombieattack.obj
fi