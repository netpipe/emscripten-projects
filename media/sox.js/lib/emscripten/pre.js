/*
 * sox.js
 * https://github.com/skratchdot/sox.js
 *
 * Copyright (c) 2014 skratchdot
 * Licensed under the MIT license.
 */
var util = require('util');

var getType = function (obj) {
	return (/(\[object )([^\]]*)(\])/).exec(({}).toString.call(obj))[2];
};

exports.sox = function(args, inputFiles, callback) {
	var filenames, file, Module = {
		'sox_stdout': '',
		'sox_files_root': '/',
		'sox_files_input': {},
		'sox_files_output': []
	};

	// setup arguments
	Module['arguments'] = [];
	if (util.isArray(args)) {
		args.forEach(function (arg) {
			if (typeof arg === 'object') {
				filenames = Object.keys(arg);
				filenames.forEach(function (filename) {
					var file = arg[filename];
					if (getType(file) === 'Uint8Array') {
						Module['sox_files_input'][filename] = file;
					} else {
						Module['sox_files_output'].push(filename);
					}
					arg = Module['sox_files_root'] + filename;
				});
			}
			Module['arguments'].push(arg);
		});
	}

	// store stdout
	Module['print'] = function (text) {
		Module['sox_stdout'] += text + '\n';
	};

	Module['preRun'] = function () {
		// create input files
		Object.keys(Module['sox_files_input']).forEach(function (filename) {
			var file = Module['sox_files_input'][filename];
			FS.createDataFile(Module['sox_files_root'], filename,  file, true, true);
		});
	};

	Module['postRun'] = function () {
		var result, outputFiles = {};
		if (typeof callback === 'function') {
  			// return output files
			result = FS.analyzePath(Module['sox_files_root']);
			if (result && result.object && result.object.contents) {
				Object.keys(result.object.contents).forEach(function (filename) {
					obj = result.object.contents[filename];
					if (Module['sox_files_output'].indexOf(filename) >= 0) {
						outputFiles[filename] = new Uint8Array(obj.contents);
					}
				});
			}

			// return data to our callback function
			callback({
				'stdout': Module['sox_stdout'],
				'outputFiles': outputFiles,
				'code': EXITSTATUS
			});
		}
    };
//}