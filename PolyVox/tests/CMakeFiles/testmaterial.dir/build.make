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
CMAKE_SOURCE_DIR = /home/netpipe/Desktop/emscripten-projects/PolyVox

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/netpipe/Desktop/emscripten-projects/PolyVox

# Include any dependencies generated for this target.
include tests/CMakeFiles/testmaterial.dir/depend.make

# Include the progress variables for this target.
include tests/CMakeFiles/testmaterial.dir/progress.make

# Include the compile flags for this target's objects.
include tests/CMakeFiles/testmaterial.dir/flags.make

tests/moc_testmaterial.cxx: tests/testmaterial.h
tests/moc_testmaterial.cxx: tests/moc_testmaterial.cxx_parameters
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/netpipe/Desktop/emscripten-projects/PolyVox/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Generating moc_testmaterial.cxx"
	cd /home/netpipe/Desktop/emscripten-projects/PolyVox/tests && /usr/bin/moc @/home/netpipe/Desktop/emscripten-projects/PolyVox/tests/moc_testmaterial.cxx_parameters

tests/CMakeFiles/testmaterial.dir/testmaterial.cpp.o: tests/CMakeFiles/testmaterial.dir/flags.make
tests/CMakeFiles/testmaterial.dir/testmaterial.cpp.o: tests/testmaterial.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/netpipe/Desktop/emscripten-projects/PolyVox/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object tests/CMakeFiles/testmaterial.dir/testmaterial.cpp.o"
	cd /home/netpipe/Desktop/emscripten-projects/PolyVox/tests && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/testmaterial.dir/testmaterial.cpp.o -c /home/netpipe/Desktop/emscripten-projects/PolyVox/tests/testmaterial.cpp

tests/CMakeFiles/testmaterial.dir/testmaterial.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/testmaterial.dir/testmaterial.cpp.i"
	cd /home/netpipe/Desktop/emscripten-projects/PolyVox/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/netpipe/Desktop/emscripten-projects/PolyVox/tests/testmaterial.cpp > CMakeFiles/testmaterial.dir/testmaterial.cpp.i

tests/CMakeFiles/testmaterial.dir/testmaterial.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/testmaterial.dir/testmaterial.cpp.s"
	cd /home/netpipe/Desktop/emscripten-projects/PolyVox/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/netpipe/Desktop/emscripten-projects/PolyVox/tests/testmaterial.cpp -o CMakeFiles/testmaterial.dir/testmaterial.cpp.s

tests/CMakeFiles/testmaterial.dir/testmaterial.cpp.o.requires:

.PHONY : tests/CMakeFiles/testmaterial.dir/testmaterial.cpp.o.requires

tests/CMakeFiles/testmaterial.dir/testmaterial.cpp.o.provides: tests/CMakeFiles/testmaterial.dir/testmaterial.cpp.o.requires
	$(MAKE) -f tests/CMakeFiles/testmaterial.dir/build.make tests/CMakeFiles/testmaterial.dir/testmaterial.cpp.o.provides.build
.PHONY : tests/CMakeFiles/testmaterial.dir/testmaterial.cpp.o.provides

tests/CMakeFiles/testmaterial.dir/testmaterial.cpp.o.provides.build: tests/CMakeFiles/testmaterial.dir/testmaterial.cpp.o


tests/CMakeFiles/testmaterial.dir/moc_testmaterial.cxx.o: tests/CMakeFiles/testmaterial.dir/flags.make
tests/CMakeFiles/testmaterial.dir/moc_testmaterial.cxx.o: tests/moc_testmaterial.cxx
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/netpipe/Desktop/emscripten-projects/PolyVox/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object tests/CMakeFiles/testmaterial.dir/moc_testmaterial.cxx.o"
	cd /home/netpipe/Desktop/emscripten-projects/PolyVox/tests && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/testmaterial.dir/moc_testmaterial.cxx.o -c /home/netpipe/Desktop/emscripten-projects/PolyVox/tests/moc_testmaterial.cxx

tests/CMakeFiles/testmaterial.dir/moc_testmaterial.cxx.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/testmaterial.dir/moc_testmaterial.cxx.i"
	cd /home/netpipe/Desktop/emscripten-projects/PolyVox/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/netpipe/Desktop/emscripten-projects/PolyVox/tests/moc_testmaterial.cxx > CMakeFiles/testmaterial.dir/moc_testmaterial.cxx.i

tests/CMakeFiles/testmaterial.dir/moc_testmaterial.cxx.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/testmaterial.dir/moc_testmaterial.cxx.s"
	cd /home/netpipe/Desktop/emscripten-projects/PolyVox/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/netpipe/Desktop/emscripten-projects/PolyVox/tests/moc_testmaterial.cxx -o CMakeFiles/testmaterial.dir/moc_testmaterial.cxx.s

tests/CMakeFiles/testmaterial.dir/moc_testmaterial.cxx.o.requires:

.PHONY : tests/CMakeFiles/testmaterial.dir/moc_testmaterial.cxx.o.requires

tests/CMakeFiles/testmaterial.dir/moc_testmaterial.cxx.o.provides: tests/CMakeFiles/testmaterial.dir/moc_testmaterial.cxx.o.requires
	$(MAKE) -f tests/CMakeFiles/testmaterial.dir/build.make tests/CMakeFiles/testmaterial.dir/moc_testmaterial.cxx.o.provides.build
.PHONY : tests/CMakeFiles/testmaterial.dir/moc_testmaterial.cxx.o.provides

tests/CMakeFiles/testmaterial.dir/moc_testmaterial.cxx.o.provides.build: tests/CMakeFiles/testmaterial.dir/moc_testmaterial.cxx.o


# Object files for target testmaterial
testmaterial_OBJECTS = \
"CMakeFiles/testmaterial.dir/testmaterial.cpp.o" \
"CMakeFiles/testmaterial.dir/moc_testmaterial.cxx.o"

# External object files for target testmaterial
testmaterial_EXTERNAL_OBJECTS =

tests/testmaterial: tests/CMakeFiles/testmaterial.dir/testmaterial.cpp.o
tests/testmaterial: tests/CMakeFiles/testmaterial.dir/moc_testmaterial.cxx.o
tests/testmaterial: tests/CMakeFiles/testmaterial.dir/build.make
tests/testmaterial: /usr/lib64/libQtTest.so
tests/testmaterial: /usr/lib64/libQtCore.so
tests/testmaterial: tests/CMakeFiles/testmaterial.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/netpipe/Desktop/emscripten-projects/PolyVox/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX executable testmaterial"
	cd /home/netpipe/Desktop/emscripten-projects/PolyVox/tests && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/testmaterial.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
tests/CMakeFiles/testmaterial.dir/build: tests/testmaterial

.PHONY : tests/CMakeFiles/testmaterial.dir/build

tests/CMakeFiles/testmaterial.dir/requires: tests/CMakeFiles/testmaterial.dir/testmaterial.cpp.o.requires
tests/CMakeFiles/testmaterial.dir/requires: tests/CMakeFiles/testmaterial.dir/moc_testmaterial.cxx.o.requires

.PHONY : tests/CMakeFiles/testmaterial.dir/requires

tests/CMakeFiles/testmaterial.dir/clean:
	cd /home/netpipe/Desktop/emscripten-projects/PolyVox/tests && $(CMAKE_COMMAND) -P CMakeFiles/testmaterial.dir/cmake_clean.cmake
.PHONY : tests/CMakeFiles/testmaterial.dir/clean

tests/CMakeFiles/testmaterial.dir/depend: tests/moc_testmaterial.cxx
	cd /home/netpipe/Desktop/emscripten-projects/PolyVox && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/netpipe/Desktop/emscripten-projects/PolyVox /home/netpipe/Desktop/emscripten-projects/PolyVox/tests /home/netpipe/Desktop/emscripten-projects/PolyVox /home/netpipe/Desktop/emscripten-projects/PolyVox/tests /home/netpipe/Desktop/emscripten-projects/PolyVox/tests/CMakeFiles/testmaterial.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : tests/CMakeFiles/testmaterial.dir/depend
