# move to source folder
cd ../sox-14.4.1

# remove 'error' line in formats.c
ERROR_FILE="./src/formats.c"
ERROR_LINE=$(sed "420q;d" "$ERROR_FILE")
if [ "$ERROR_LINE" == "  #error FIX NEEDED HERE" ]; then
  sed -i.bak -e "420d" "$ERROR_FILE"
fi

# dumb patch b/c --no-glob doesn't seem to work, and _glob() is
# undefined when writing output files after compiling w/ emscripten
GLOB_FIX_FILE="./src/sox.c"
GLOB_FIX_LINE=$(sed "2598q;d" "$GLOB_FIX_FILE")
if [ "$GLOB_FIX_LINE" == "#if HAVE_GLOB_H" ]; then
  cp "$GLOB_FIX_FILE" "$GLOB_FIX_FILE".bak
  sed -i "2598,2624d" "$GLOB_FIX_FILE"
  sed -i "2598i#define add_glob_file add_file" "$GLOB_FIX_FILE"
fi

# clean environment
make clean
make distclean

# configure
emconfigure ./configure \
CC="emcc" \
--with-distro="sox.js - emscripten - http://github.com/skratchdot/sox.js"
--without-png \
--without-ladspa \
--disable-symlinks

# make
emmake make

# build js
emcc -O2 src/sox.o src/.libs/*.so src/.libs/*.a ./libgsm/.libs/*.a ./lpc10/.libs/*.a \
--pre-js /vagrant/lib/emscripten/pre.js \
--post-js /vagrant/lib/emscripten/post.js  \
-o /vagrant/lib/emscripten/sox.js

emcc -O2 src/sox.o src/.libs/*.so src/.libs/*.a ./libgsm/.libs/*.a ./lpc10/.libs/*.a -o sox.bc

emcc -O2 src/sox.o src/.libs/*.so src/.libs/*.a ./libgsm/.libs/*.a ./lpc10/.libs/*.a -o sox.html


##############
#https://groups.google.com/forum/#!topic/emscripten-discuss/Krkobypk_dw
#
#warning: unresolved symbol: glob
#warning: unresolved symbol: globfree
#warning: unresolved symbol: fmemopen
#warning: unresolved symbol: open_memstream
#
#
#var _add_glob_file = _add_file;
#function _add_glob_fileXXXXXXXXX($opts,$filename){
#
##############
