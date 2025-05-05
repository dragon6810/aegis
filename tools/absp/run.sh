#!/usr/bin/env bash

if make; then
    cd ../run
    ../absp/bin/absp -v zombieattack
fi