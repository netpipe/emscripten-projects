irrlicht-ogl-es - Emscripten port
=================================

Building
--------

Download and install Emscripten <http://kripken.github.io/emscripten-site/docs/getting_started/downloads.html>

Build Irrlicht: 

```
make irrlicht
```

Build the 01.HelloWorld example:

```
cd irrlicht/examples/01.HelloWorld
emmake make -f Makefile.emscripten
```

Run the example with Firefox (doesn't work with Chrome):

```
cd ../../bin/Linux
emrun --browser firefox 01.HelloWorld.html
```

