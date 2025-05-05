#!/usr/bin/env bash

if make; then
    cd ../run
    ../acsg/bin/acsg zombieattack
    python3 ../acsg/gtoobj.py zombieattack.g0 zombieattack0.obj
    python3 ../acsg/gtoobj.py zombieattack.g1 zombieattack1.obj
    python3 ../acsg/gtoobj.py zombieattack.g2 zombieattack2.obj
    python3 ../acsg/gtoobj.py zombieattack.g3 zombieattack3.obj
fi