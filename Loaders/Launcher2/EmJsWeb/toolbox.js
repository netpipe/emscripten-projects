var toolJs = "EmWasmTool.js";
var staticLoaded = false;
var wasm_args = "";

// Comment out the following line to opt for dynamic js loading
staticLoadJs(toolJs);

function launchWasmTool(text_inp) {
    wasm_args = text_inp;
    if (staticLoaded) {
        toolCode();
    } else {
        dynamicLoadJs(toolJs, toolCode);
    }
}

var toolCode = function () {
    wasm_tool = Module.cwrap('str_conv', 'string', ['string']);
    document.getElementById("output").innerHTML=wasm_tool(wasm_args).replaceAll("\n", "<br/>");
};

function staticLoadJs(url) {
    var script = document.createElement('script');
    script.src = url;
    script.type = 'text/javascript';
    script.defer = true;
    document.getElementsByTagName('head').item(0).appendChild(script);
    staticLoaded = true;
}

function dynamicLoadJs(url, callback) {
    var head = document.head;
    var script = document.createElement('script');
    script.type = 'text/javascript';
    script.src = url;
    script.onreadystatechange = callback;
    script.onload = callback;
    head.appendChild(script);
}
