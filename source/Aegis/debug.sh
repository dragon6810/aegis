#!/usr/bin/env bash

cd build
if make
then
	cd ..
	gdb ./build/Aegis
else
	echo ERROR: Failed to build project
	exit 1
fi

exit 0
