#!/usr/bin/env bash
#source /home/Dev/libs/Scripting/emsdk/emsdk_env.sh
set -ue

EMSCRIPTEN_USR_DIR=$PWD/build
LIBTAR_DIR=$PWD/../libtar
BUILD_TRIPPLE=$(${LIBTAR_DIR}/autoconf/config.guess)

pushd "${LIBTAR_DIR}"
emconfigure ./configure --disable-shared --enable-static --host=asmjs-unknown-emscripten --build=${BUILD_TRIPPLE} --prefix=${EMSCRIPTEN_USR_DIR}
emmake make
emmake make -C lib install
#make distclean
#rm -f a.out a.out.js
#popd



#found this to work aswell
#BASECFLAGS=-m32 LDFLAGS=-m32  emconfigure ./configure --host=asmjs-unknown-emscripten
