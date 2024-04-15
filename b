#!/bin/bash

rm -rf build
mkdir build
cd build
#cmake .. -DCMAKE_BUILD_TYPE=Debug -DMYPROJ_BUILD_TESTS=ON
cmake .. -DCMAKE_BUILD_TYPE=Debug
#cmake .. -DCMAKE_BUILD_TYPE=Coverage
