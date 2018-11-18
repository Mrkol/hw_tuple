#!/bin/bash

echo "Rebuilding the whole project from scratch..."

if [ -d "./build" ]; then
	rm -Rf ./build
fi

mkdir build

cd build

cmake $@ ../

cmake --build .

cd ..
