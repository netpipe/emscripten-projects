
cd assimp-3.3.1
emcmake cmake . -DBUILD_SHARED_LIBS=OFF
cp ./lib/libassimp.a ../bin/
#libzlibstatic.a
cd ..



put new glfw into opensteer and then copied osmesa.h to GL folder inside source tree
compiled with emcmake cmake -DGLFW_USE_OSMESA=1
