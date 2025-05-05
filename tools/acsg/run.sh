#!/usr/bin/env bash

if make; then
    cd ../run
    ../acsg/bin/acsg zombieattack
    python3 ../acsg/gtoobj.py zombieattack.g0 zombieattack.obj
fi