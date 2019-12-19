#!/bin/bash
cd bullet

emcmake cmake . -DBUILD_DEMOS=OFF -DBUILD_EXTRAS=OFF -DBUILD_CPU_DEMOS=OFF -DUSE_GLUT=OFF -DCMAKE_BUILD_TYPE=Release
 make
