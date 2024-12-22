#!bin/bash

cd build
if make; then
	cd ..
	./build/Aegis
else
	exit 1
fi

exit 0
