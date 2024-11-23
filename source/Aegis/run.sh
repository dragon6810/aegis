#!bin/bash

cd build
if make
then
	cd ..
	./build/Aegis
else
	echo ERROR: Failed to build project
	exit 1
fi

exit 0
