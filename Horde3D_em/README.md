horde3d-em
==========
Horde3D is a small open source 3D rendering engine.

This is a fork of https://github.com/attilaz/horde3d-x/tree/gles2_d3d11
which is based on horde3d beta5. This can be compiled to js/wasm with emscripten.

Horde3D/Samples/ directory contains 3 makefiles and .bat file.
Edit h3d_compile.bat if needed.
h3d_compile.bat takes makefile as a parameter ie
h3d_compile.bat Makefile_emscripten_DEBUG

---

Small wasm test at
Horde3D/Samples/h3d_test.html

---

* removed original samples and makefiles
* removed gl and d3d11 (this is only gles2+emscripten)
* removed c# bindings

* added new sample using sdl2  (Horde3D/Samples/Test_1.cpp Horde3D/Samples/Framework.h)


---

Issues:
* no shadows
* dds images doesnt load (so no sky and reflection because those uses cubemap dds)

---

Copyright (C) 2006-2018 Nicolas Schulz and the Horde3D Team
http://www.horde3d.org

The complete SDK is licensed under the terms of the Eclipse Public License (EPL).
