#!/bin/bash

rm -rf ./build/
cmake -DCMAKE_BUILD_TYPE:STRING=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_C_COMPILER:FILEPATH=/usr/bin/gcc-9 -DCMAKE_CXX_COMPILER:FILEPATH=/usr/bin/g++-9 -S/home/ubuntu/cpp_projects/cmake_template/cmake-debug-template -B/home/ubuntu/cpp_projects/cmake_template/cmake-debug-template/build -G "Unix Makefiles"
cd ./build && make