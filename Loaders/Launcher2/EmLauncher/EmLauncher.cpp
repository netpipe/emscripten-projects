#include <emscripten.h>
#include <emscripten/bind.h>
using namespace emscripten;

void launch() {
    EM_ASM(
        var text_inp = document.getElementById("tid").value;
        document.getElementById('ifrm').contentWindow.launchWasmTool(text_inp);
    );
}

EMSCRIPTEN_BINDINGS(my_module) {
    function("launch", &launch);
}

