###The following changes have been made to 8086tiny:

* Using stdio in browser via emscripten blocks execution. SDL keyboard events dont. So, instead of switching back and forth between stdio in text mode and SDL in graphical mode, we use a small persistent SDL window to always be monitoring SDL events for keyboard input.
* 8086tiny assumes that `SDLKey` enumeration's values map to ASCII keycodes. This is not guaranteed by the specification, and does not hold on emscripten in browsers. Thus, a funtion to map `SDLKey` values to correct ASCII keycodes is defined.
* Emscripten in browsers does not support unbuffered output, nor does the console support ANSI escape codes. To emulate unbuffered output and ANSI escape codes, the virtual terminal library [libtmt](https://github.com/deadpixi/libtmt) is used. The screen of the virtual terminal is dumped into the console along with some blank lines to emulate the desired behavior. This must be enabled by the `-DUSE_TMT` compile time flag.
