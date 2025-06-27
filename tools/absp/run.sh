#!/usr/bin/env bash

if make; then
    cd ../run
    ../absp/bin/absp -v -k zombieattack
fi