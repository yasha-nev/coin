#! /bin/bash

cd /home/;

cmake -B build

cmake --build build -j

./build/main 5060