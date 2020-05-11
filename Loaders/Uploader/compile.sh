emcc displayInfo.cpp -std=c++11 -s 'EXPORTED_FUNCTIONS=["_addFile","_processFiles","_main"]' -s 'EXTRA_EXPORTED_RUNTIME_METHODS=["cwrap"]' -s FORCE_FILESYSTEM=1 -o display.html
