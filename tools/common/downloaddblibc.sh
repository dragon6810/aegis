#!/usr/bin/env bash

rm -rf std

mkdir -p lib
rm -rf lib/dblibc
cd lib
git clone https://github.com/dragon6810/dblibc.git

cd ..
mv lib/dblibc/std std
rm -rf lib