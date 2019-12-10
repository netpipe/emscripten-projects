
# FluidSynth with Emscripten-specific patch

This repository is based on [FluidSynth](https://github.com/FluidSynth/fluidsynth) repository, and contains some changes to build with Emscripten.

The original README is here: [README.original.md](./README.original.md)

## Build

1. (Optional) Update `emscripten/exports.txt`, containing export functions for JS program
    * The script `emscripten/make-exports.js` will update this automatically, gathering functions from `include` directory.
2. Make sure that Emscripten is usable on the current environment
3. Make `build` directory
4. Enter `build` directory and execute `emcmake cmake ..`
    * If no options are specified, and `cmake` is running with `emcmake` (or `emconfigure`), the build configurations are initialized for Emscripten-build mode.
5. Make `src/gentables` directory in the current `build` directory
6. Enter `src/gentables` directory (`build/src/gentables` from the root) and execute `cmake ../../../src/gentables`
    * This calls original CMake to build native `make_tables` program. Do not use `emcmake` here.
7. In `build/src/gentables` directory, execute `make`. After successful, execute `./make_tables ../../` (do not forget trailing `/` character).
    * This is the build step originally written in `src/CMakeLists.txt`.
8. Return to `build` directory (`cd ../..`) and execute `emmake make`

After successful build, `libfluidsynth-<version>.js` will be created at `build/src` directory.

* If `enable-debug` specified on the `cmake` execution (e.g. `emcmake cmake -Denable-debug=on ..`), a map file `libfluidsynth-<version>.wasm.map` is also generated.
    * Currently it seems that it cannot be used.
* If `enable-separate-wasm` specified on the `cmake` execution (e.g. `emcmake cmake -Denable-separate-wasm=on ..`), `libfluidsynth-<version>.wasm` and `libfluidsynth-<version>.wast` are also generated.
    * For AudioWorklet, you cannot use `*.wasm` file directly.
* In Emscripten-build mode, standalone application named `fluidsynth` is not emitted.

## Usage

Place `libfluidsynth-<version>.js` file to your space and load `libfluidsynth-<version>.js`. After load, almost all FluidSynth API functions are accessible via `Module` object (note that all function names have the prefix `_`).

To use `libfluidsynth-<version>.js` in AudioWorklet, load it into AudioWorklet before your worklet JS file. In your worklet JS file, you can access `Module` object via `AudioWorkletGlobalScope.wasmModule`.

## Miscellaneous

* Currently only several APIs are tested. Some APIs such as for drivers may not work.

## License

This program and all source codes, including the original FluidSynth program, its source codes, modifications of FluidSynth source codes for building library with Emscripten, and sources codes used only for building `libfluidsynth-<version>.js`, are licensed under [GNU Lesser General Public License (v2.1)](./LICENSE) (LGPL v2.1).
