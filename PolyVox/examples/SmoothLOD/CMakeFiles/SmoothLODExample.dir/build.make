# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/emscripten-projects/PolyVox

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/emscripten-projects/PolyVox

# Include any dependencies generated for this target.
include examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/depend.make

# Include the progress variables for this target.
include examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/progress.make

# Include the compile flags for this target's objects.
include examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/flags.make

examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/glew/glew.cpp.o: examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/flags.make
examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/glew/glew.cpp.o: examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/includes_CXX.rsp
examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/glew/glew.cpp.o: examples/SmoothLOD/glew/glew.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/emscripten-projects/PolyVox/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/glew/glew.cpp.o"
	cd /run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/emscripten-projects/PolyVox/examples/SmoothLOD && /run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/Dev/libs/Scripting/emsdk/upstream/emscripten/em++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/SmoothLODExample.dir/glew/glew.cpp.o -c /run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/emscripten-projects/PolyVox/examples/SmoothLOD/glew/glew.cpp

examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/glew/glew.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/SmoothLODExample.dir/glew/glew.cpp.i"
	cd /run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/emscripten-projects/PolyVox/examples/SmoothLOD && /run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/Dev/libs/Scripting/emsdk/upstream/emscripten/em++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/emscripten-projects/PolyVox/examples/SmoothLOD/glew/glew.cpp > CMakeFiles/SmoothLODExample.dir/glew/glew.cpp.i

examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/glew/glew.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/SmoothLODExample.dir/glew/glew.cpp.s"
	cd /run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/emscripten-projects/PolyVox/examples/SmoothLOD && /run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/Dev/libs/Scripting/emsdk/upstream/emscripten/em++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/emscripten-projects/PolyVox/examples/SmoothLOD/glew/glew.cpp -o CMakeFiles/SmoothLODExample.dir/glew/glew.cpp.s

examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/glew/glew.cpp.o.requires:

.PHONY : examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/glew/glew.cpp.o.requires

examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/glew/glew.cpp.o.provides: examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/glew/glew.cpp.o.requires
	$(MAKE) -f examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/build.make examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/glew/glew.cpp.o.provides.build
.PHONY : examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/glew/glew.cpp.o.provides

examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/glew/glew.cpp.o.provides.build: examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/glew/glew.cpp.o


examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/main.cpp.o: examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/flags.make
examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/main.cpp.o: examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/includes_CXX.rsp
examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/main.cpp.o: examples/SmoothLOD/main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/emscripten-projects/PolyVox/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/main.cpp.o"
	cd /run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/emscripten-projects/PolyVox/examples/SmoothLOD && /run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/Dev/libs/Scripting/emsdk/upstream/emscripten/em++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/SmoothLODExample.dir/main.cpp.o -c /run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/emscripten-projects/PolyVox/examples/SmoothLOD/main.cpp

examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/SmoothLODExample.dir/main.cpp.i"
	cd /run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/emscripten-projects/PolyVox/examples/SmoothLOD && /run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/Dev/libs/Scripting/emsdk/upstream/emscripten/em++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/emscripten-projects/PolyVox/examples/SmoothLOD/main.cpp > CMakeFiles/SmoothLODExample.dir/main.cpp.i

examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/SmoothLODExample.dir/main.cpp.s"
	cd /run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/emscripten-projects/PolyVox/examples/SmoothLOD && /run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/Dev/libs/Scripting/emsdk/upstream/emscripten/em++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/emscripten-projects/PolyVox/examples/SmoothLOD/main.cpp -o CMakeFiles/SmoothLODExample.dir/main.cpp.s

examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/main.cpp.o.requires:

.PHONY : examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/main.cpp.o.requires

examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/main.cpp.o.provides: examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/main.cpp.o.requires
	$(MAKE) -f examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/build.make examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/main.cpp.o.provides.build
.PHONY : examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/main.cpp.o.provides

examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/main.cpp.o.provides.build: examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/main.cpp.o


examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/OpenGLWidget.cpp.o: examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/flags.make
examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/OpenGLWidget.cpp.o: examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/includes_CXX.rsp
examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/OpenGLWidget.cpp.o: examples/SmoothLOD/OpenGLWidget.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/emscripten-projects/PolyVox/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/OpenGLWidget.cpp.o"
	cd /run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/emscripten-projects/PolyVox/examples/SmoothLOD && /run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/Dev/libs/Scripting/emsdk/upstream/emscripten/em++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/SmoothLODExample.dir/OpenGLWidget.cpp.o -c /run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/emscripten-projects/PolyVox/examples/SmoothLOD/OpenGLWidget.cpp

examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/OpenGLWidget.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/SmoothLODExample.dir/OpenGLWidget.cpp.i"
	cd /run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/emscripten-projects/PolyVox/examples/SmoothLOD && /run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/Dev/libs/Scripting/emsdk/upstream/emscripten/em++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/emscripten-projects/PolyVox/examples/SmoothLOD/OpenGLWidget.cpp > CMakeFiles/SmoothLODExample.dir/OpenGLWidget.cpp.i

examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/OpenGLWidget.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/SmoothLODExample.dir/OpenGLWidget.cpp.s"
	cd /run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/emscripten-projects/PolyVox/examples/SmoothLOD && /run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/Dev/libs/Scripting/emsdk/upstream/emscripten/em++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/emscripten-projects/PolyVox/examples/SmoothLOD/OpenGLWidget.cpp -o CMakeFiles/SmoothLODExample.dir/OpenGLWidget.cpp.s

examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/OpenGLWidget.cpp.o.requires:

.PHONY : examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/OpenGLWidget.cpp.o.requires

examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/OpenGLWidget.cpp.o.provides: examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/OpenGLWidget.cpp.o.requires
	$(MAKE) -f examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/build.make examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/OpenGLWidget.cpp.o.provides.build
.PHONY : examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/OpenGLWidget.cpp.o.provides

examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/OpenGLWidget.cpp.o.provides.build: examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/OpenGLWidget.cpp.o


# Object files for target SmoothLODExample
SmoothLODExample_OBJECTS = \
"CMakeFiles/SmoothLODExample.dir/glew/glew.cpp.o" \
"CMakeFiles/SmoothLODExample.dir/main.cpp.o" \
"CMakeFiles/SmoothLODExample.dir/OpenGLWidget.cpp.o"

# External object files for target SmoothLODExample
SmoothLODExample_EXTERNAL_OBJECTS =

examples/SmoothLOD/SmoothLODExample.js: examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/glew/glew.cpp.o
examples/SmoothLOD/SmoothLODExample.js: examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/main.cpp.o
examples/SmoothLOD/SmoothLODExample.js: examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/OpenGLWidget.cpp.o
examples/SmoothLOD/SmoothLODExample.js: examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/build.make
examples/SmoothLOD/SmoothLODExample.js: examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/linklibs.rsp
examples/SmoothLOD/SmoothLODExample.js: examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/objects1.rsp
examples/SmoothLOD/SmoothLODExample.js: examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/emscripten-projects/PolyVox/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX executable SmoothLODExample.js"
	cd /run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/emscripten-projects/PolyVox/examples/SmoothLOD && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/SmoothLODExample.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/build: examples/SmoothLOD/SmoothLODExample.js

.PHONY : examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/build

examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/requires: examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/glew/glew.cpp.o.requires
examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/requires: examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/main.cpp.o.requires
examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/requires: examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/OpenGLWidget.cpp.o.requires

.PHONY : examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/requires

examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/clean:
	cd /run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/emscripten-projects/PolyVox/examples/SmoothLOD && $(CMAKE_COMMAND) -P CMakeFiles/SmoothLODExample.dir/cmake_clean.cmake
.PHONY : examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/clean

examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/depend:
	cd /run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/emscripten-projects/PolyVox && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/emscripten-projects/PolyVox /run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/emscripten-projects/PolyVox/examples/SmoothLOD /run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/emscripten-projects/PolyVox /run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/emscripten-projects/PolyVox/examples/SmoothLOD /run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/emscripten-projects/PolyVox/examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : examples/SmoothLOD/CMakeFiles/SmoothLODExample.dir/depend

