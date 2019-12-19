#!/bin/bash


#rm cmakecache
mkdir build
cd build
emcmake cmake ..
make -j4

mkdir src/gentables

cd src/gentables
cmake ../../../src/gentables
make
./make_tables ../../
cd ../..
emmake make
