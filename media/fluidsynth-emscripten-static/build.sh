#!/bin/bash

source /home/Dev/libs/Scripting/emsdk/emsdk_env.sh

mkdir build
cd build
emcmake cmake -D BUILD_SHARED_LIBS=off ..
make -j4

