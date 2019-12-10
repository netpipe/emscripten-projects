#!/usr/bin/env bash
source /home/Dev/libs/Scripting/emsdk/emsdk_env.sh

set -ue

EMSCRIPTEN_USR_DIR=$PWD/emscripten
PYTHON_DIR=$PWD/Python-2.7
BUILD_TRIPPLE=$(${PYTHON_DIR}/config.guess)

OSNAME=$(uname -s)
if [ "${OSNAME}" == "Linux" ]
then
	PYTHON_BINARY=python
else
	PYTHON_BINARY=python.exe
fi

pushd "${PYTHON_DIR}"

if [ ! -f "${PYTHON_BINARY}" ]
then
	./configure
	make ${PYTHON_BINARY} Parser/pgen
	mv ${PYTHON_BINARY} hostpython
	mv Parser/pgen Parser/hostpgen
	make distclean
fi

CONFIG_SITE=./config.site; emconfigure ./configure --without-threads --without-pymalloc --disable-shared --disable-ipv6 --without-gcc --host=asmjs-unknown-emscripten --build=${BUILD_TRIPPLE} --prefix=${EMSCRIPTEN_USR_DIR}

#BUILD_TRIPPLE=/home/netpipe/Desktop/EMScripten-AddonBundle/Libs/Scripting/EmCPython/Python-2.7/config.guess;CONFIG_SITE=./config.site emconfigure ./configure --without-threads --without-pymalloc --disable-shared --disable-ipv6 --without-gcc --host=asmjs-unknown-emscripten --build=${BUILD_TRIPPLE} --prefix=/home/Dev/libs/Scripting/emsdk/upstream/emscripten/emscripten

#BUILD_TRIPPLE=../Python-2.7/config.guess;CONFIG_SITE=./config.site emconfigure ./configure --without-threads --without-pymalloc --disable-shared --disable-ipv6 --without-gcc --host=asmjs-unknown-emscripten --build=../Python-2.7/config.guess --prefix=/home/Dev/libs/Scripting/emsdk/upstream/emscripten/emscripten



cp ../Setup.local Modules/
emmake make HOSTPYTHON=./hostpython HOSTPGEN=./Parser/hostpgen CROSS_COMPILE=yes
emmake make install HOSTPYTHON=./hostpython HOSTPGEN=./Parser/hostpgen CROSS_COMPILE=yes
#make distclean
#popd

