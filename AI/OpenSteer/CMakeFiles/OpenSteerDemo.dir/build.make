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
CMAKE_SOURCE_DIR = /run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/EMLibs/OpenSteer

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/EMLibs/OpenSteer

# Include any dependencies generated for this target.
include CMakeFiles/OpenSteerDemo.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/OpenSteerDemo.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/OpenSteerDemo.dir/flags.make

# Object files for target OpenSteerDemo
OpenSteerDemo_OBJECTS =

# External object files for target OpenSteerDemo
OpenSteerDemo_EXTERNAL_OBJECTS =

OpenSteerDemo.js: CMakeFiles/OpenSteerDemo.dir/build.make
OpenSteerDemo.js: liblibopensteer.a
OpenSteerDemo.js: third-party/glfw/src/libglfw3.a
OpenSteerDemo.js: CMakeFiles/OpenSteerDemo.dir/linklibs.rsp
OpenSteerDemo.js: CMakeFiles/OpenSteerDemo.dir/objects1.rsp
OpenSteerDemo.js: CMakeFiles/OpenSteerDemo.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/EMLibs/OpenSteer/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Linking CXX executable OpenSteerDemo.js"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/OpenSteerDemo.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/OpenSteerDemo.dir/build: OpenSteerDemo.js

.PHONY : CMakeFiles/OpenSteerDemo.dir/build

CMakeFiles/OpenSteerDemo.dir/requires:

.PHONY : CMakeFiles/OpenSteerDemo.dir/requires

CMakeFiles/OpenSteerDemo.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/OpenSteerDemo.dir/cmake_clean.cmake
.PHONY : CMakeFiles/OpenSteerDemo.dir/clean

CMakeFiles/OpenSteerDemo.dir/depend:
	cd /run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/EMLibs/OpenSteer && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/EMLibs/OpenSteer /run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/EMLibs/OpenSteer /run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/EMLibs/OpenSteer /run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/EMLibs/OpenSteer /run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/EMLibs/OpenSteer/CMakeFiles/OpenSteerDemo.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/OpenSteerDemo.dir/depend

