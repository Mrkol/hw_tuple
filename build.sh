#!/bin/bash

echo Building the project...

if [ ! -d "./build" ]; then
	mkdir ./build
fi

cd build

cmake $@ ../

cmake --build .

cd ..
