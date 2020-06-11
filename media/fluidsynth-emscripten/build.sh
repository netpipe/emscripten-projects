#!/bin/bash

rm src/gentables/CMakeCache.txt
#rm cmakecache
mkdir build
cd build
emcmake cmake .. -DBUILD_SHARED_LIBS=false -DBUILD_STATIC_LIBS=true
#emcmake cmake ..
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


