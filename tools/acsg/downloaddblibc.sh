#!/usr/bin/env bash

mkdir -p lib
rm -rf lib/dblibc
cd lib
git clone https://github.com/dragon6810/dblibc.git