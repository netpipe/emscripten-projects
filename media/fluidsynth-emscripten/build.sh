#!/bin/bash


#rm cmakecache
mkdir build
cd build
emcmake cmake .. -DBUILD_SHARED_LIBS=false
#make -j4

mkdir src
cd src
mkdir gentables
cd gentables
#cd src/gentables
cmake ../../../src/gentables
make
./make_tables ../../
cd ../..
emmake make


