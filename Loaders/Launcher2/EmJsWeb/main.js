var http = require('http');
var fs = require('fs');

var htmlFile;
var tbHtmlFile;
var tbJsFile;

var elJsFile;
var elWasmFile;
var ewtJsFile;
var ewtWasmFile;

fs.readFile('./page.html', function (err, data) {
    if (err) throw err;
    htmlFile = data;
});

fs.readFile('./toolbox.html', function (err, data) {
    if (err) throw err;
    tbHtmlFile = data;
});

fs.readFile('./toolbox.js', function (err, data) {
    if (err) throw err;
    tbJsFile = data;
});

fs.readFile('./EmLauncher.js', function (err, data) {
    if (err) throw err;
    elJsFile = data;
});

fs.readFile('./EmLauncher.wasm', function (err, data) {
    if (err) throw err;
    elWasmFile = data;
});

fs.readFile('./EmWasmTool.js', function (err, data) {
    if (err) throw err;
    ewtJsFile = data;
});

fs.readFile('./EmWasmTool.wasm', function (err, data) {
    if (err) throw err;
    ewtWasmFile = data;
});

var server = http.createServer(function (request, response) {
    switch (request.url) {
        case "/toolbox.html" :
            response.writeHead(200, {"Content-Type": "text/html"});
            response.write(tbHtmlFile);
            break;
        case "/toolbox.js" :
            response.writeHead(200, {"Content-Type": "text/javascript"});
            response.write(tbJsFile);
            break;
        case "/EmLauncher.js" :
            response.writeHead(200, {"Content-Type": "text/javascript"});
            response.write(elJsFile);
            break;
        case "/EmLauncher.wasm" :
            response.writeHead(200, {"Content-Type": "application/wasm"});
            response.write(elWasmFile);
            break;
        case "/EmWasmTool.js" :
            response.writeHead(200, {"Content-Type": "text/javascript"});
            response.write(ewtJsFile);
            break;
        case "/EmWasmTool.wasm" :
            response.writeHead(200, {"Content-Type": "application/wasm"});
            response.write(ewtWasmFile);
            break;
        default :
            response.writeHead(200, {"Content-Type": "text/html"});
            response.write(htmlFile);
    }
    response.end();
}).listen(8080);
