##The following changes have been made to 8086tiny:

### Keyboard:

* Using stdio in browser via emscripten blocks execution. SDL keyboard events dont. So, instead of switching back and forth between stdio in text mode and SDL in graphical mode, we use a small persistent SDL window to always be monitoring SDL events for keyboard input.
* 8086tiny assumes that `SDLKey` enumeration's values map to ASCII keycodes. This is not guaranteed by the specification, and does not hold on emscripten in browsers. Thus, a funtion to map `SDLKey` values to correct ASCII keycodes is defined.
* Emscripten in browsers does not support unbuffered output, nor does the console support ANSI escape codes. To emulate unbuffered output and ANSI escape codes, the virtual terminal library [libtmt](https://github.com/deadpixi/libtmt) is used. The screen of the virtual terminal is dumped into the console along with some blank lines to emulate the desired behavior. This must be enabled by the `-DUSE_TMT` compile time flag.

### Graphics:

* 8086tiny was using a 8-bit per pixel RGB332 surface for rendering. Browser are much more comfortable handling 32-bits per pixel 8-bit per channel RGBA rendering surfaces. The emulated rendering was done by packing color data of 4 adjacent pixels in a row into an `unisigned int` in a manner that it was directly compatible with data for 4 pixels in the RGB332 surface. The code creating this `unsigned int` is intact, only the resulting int is converted into 32-bit per pixel RGBA color values for 4 pixels and rendered to a 32-bpp RGBA surface.
* `SDL_LockSurface()` and `SDL_UnlockSurface()` is used to safely allow manipulating pixel data of surfaces in browser.
* As in the browser the code runs via a callback to `main_loop()`, the `main_loop()` itself executes several instructions in a loop to maintain a high instruction per second count. The check for `GRAPHICS_UPDATE_DELAY` and rendering is done outside this loop, potentially skipping frame drawing opportunities. Thus to maintain a good framerate, the check for `GRAPHICS_UPDATE_DELAY` is disabled when compiling with emscripten.

### Makefile:

* The target `8086tiny_emsc` has been added to build the emscripten version.
* Existing targets have been modified with correct compiler options.
