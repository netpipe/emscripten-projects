# Get dependencies in place (example for Debian based systems)
#sudo apt-get build-dep --yes qt5-default
#sudo apt-get install --yes libxcb-xinerama0-dev git python cmake default-jre

# Install Emscripten
git clone https://github.com/juj/emsdk.git
cd emsdk
./emsdk install latest
./emsdk activate latest
source emsdk_env.sh
cd ..

# Build Qt
git clone -b 5.14 https://code.qt.io/qt/qt5.git
cd qt5
./init-repository -f --module-subset=qtbase,qtdeclarative,qtwebsockets,qtsvg,qtquickcontrols,qtquickcontrols2,qtgraphicaleffects
./configure -opensource -confirm-license -xplatform wasm-emscripten -release -static -no-feature-thread -nomake examples -no-dbus -no-ssl
make
cd ..

# Build your project
git clone https://github.com/msorvig/qt-webassembly-examples.git
cd qt-webassembly-examples/quick_controls2_gallery
../../../qt5/qtbase/bin/qmake
make

# Serve it and browse to http://localhost:8000/gallery.html
#python -mSimpleHTTPServer
