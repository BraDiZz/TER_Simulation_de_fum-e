# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
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
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/e20190004794/Bureau/master_imagine/s2/TER/code

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/e20190004794/Bureau/master_imagine/s2/TER/code/build

# Include any dependencies generated for this target.
include external/glfw-3.1.2/examples/CMakeFiles/gears.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include external/glfw-3.1.2/examples/CMakeFiles/gears.dir/compiler_depend.make

# Include the progress variables for this target.
include external/glfw-3.1.2/examples/CMakeFiles/gears.dir/progress.make

# Include the compile flags for this target's objects.
include external/glfw-3.1.2/examples/CMakeFiles/gears.dir/flags.make

external/glfw-3.1.2/examples/CMakeFiles/gears.dir/gears.c.o: external/glfw-3.1.2/examples/CMakeFiles/gears.dir/flags.make
external/glfw-3.1.2/examples/CMakeFiles/gears.dir/gears.c.o: ../external/glfw-3.1.2/examples/gears.c
external/glfw-3.1.2/examples/CMakeFiles/gears.dir/gears.c.o: external/glfw-3.1.2/examples/CMakeFiles/gears.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/e20190004794/Bureau/master_imagine/s2/TER/code/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object external/glfw-3.1.2/examples/CMakeFiles/gears.dir/gears.c.o"
	cd /home/e20190004794/Bureau/master_imagine/s2/TER/code/build/external/glfw-3.1.2/examples && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT external/glfw-3.1.2/examples/CMakeFiles/gears.dir/gears.c.o -MF CMakeFiles/gears.dir/gears.c.o.d -o CMakeFiles/gears.dir/gears.c.o -c /home/e20190004794/Bureau/master_imagine/s2/TER/code/external/glfw-3.1.2/examples/gears.c

external/glfw-3.1.2/examples/CMakeFiles/gears.dir/gears.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/gears.dir/gears.c.i"
	cd /home/e20190004794/Bureau/master_imagine/s2/TER/code/build/external/glfw-3.1.2/examples && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/e20190004794/Bureau/master_imagine/s2/TER/code/external/glfw-3.1.2/examples/gears.c > CMakeFiles/gears.dir/gears.c.i

external/glfw-3.1.2/examples/CMakeFiles/gears.dir/gears.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/gears.dir/gears.c.s"
	cd /home/e20190004794/Bureau/master_imagine/s2/TER/code/build/external/glfw-3.1.2/examples && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/e20190004794/Bureau/master_imagine/s2/TER/code/external/glfw-3.1.2/examples/gears.c -o CMakeFiles/gears.dir/gears.c.s

# Object files for target gears
gears_OBJECTS = \
"CMakeFiles/gears.dir/gears.c.o"

# External object files for target gears
gears_EXTERNAL_OBJECTS =

external/glfw-3.1.2/examples/gears: external/glfw-3.1.2/examples/CMakeFiles/gears.dir/gears.c.o
external/glfw-3.1.2/examples/gears: external/glfw-3.1.2/examples/CMakeFiles/gears.dir/build.make
external/glfw-3.1.2/examples/gears: external/glfw-3.1.2/src/libglfw3.a
external/glfw-3.1.2/examples/gears: /usr/lib/x86_64-linux-gnu/librt.a
external/glfw-3.1.2/examples/gears: /usr/lib/x86_64-linux-gnu/libm.so
external/glfw-3.1.2/examples/gears: /usr/lib/x86_64-linux-gnu/libX11.so
external/glfw-3.1.2/examples/gears: /usr/lib/x86_64-linux-gnu/libXrandr.so
external/glfw-3.1.2/examples/gears: /usr/lib/x86_64-linux-gnu/libXinerama.so
external/glfw-3.1.2/examples/gears: /usr/lib/x86_64-linux-gnu/libXi.so
external/glfw-3.1.2/examples/gears: /usr/lib/x86_64-linux-gnu/libXxf86vm.so
external/glfw-3.1.2/examples/gears: /usr/lib/x86_64-linux-gnu/libXcursor.so
external/glfw-3.1.2/examples/gears: /usr/lib/x86_64-linux-gnu/libGL.so
external/glfw-3.1.2/examples/gears: external/glfw-3.1.2/examples/CMakeFiles/gears.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/e20190004794/Bureau/master_imagine/s2/TER/code/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable gears"
	cd /home/e20190004794/Bureau/master_imagine/s2/TER/code/build/external/glfw-3.1.2/examples && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/gears.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
external/glfw-3.1.2/examples/CMakeFiles/gears.dir/build: external/glfw-3.1.2/examples/gears
.PHONY : external/glfw-3.1.2/examples/CMakeFiles/gears.dir/build

external/glfw-3.1.2/examples/CMakeFiles/gears.dir/clean:
	cd /home/e20190004794/Bureau/master_imagine/s2/TER/code/build/external/glfw-3.1.2/examples && $(CMAKE_COMMAND) -P CMakeFiles/gears.dir/cmake_clean.cmake
.PHONY : external/glfw-3.1.2/examples/CMakeFiles/gears.dir/clean

external/glfw-3.1.2/examples/CMakeFiles/gears.dir/depend:
	cd /home/e20190004794/Bureau/master_imagine/s2/TER/code/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/e20190004794/Bureau/master_imagine/s2/TER/code /home/e20190004794/Bureau/master_imagine/s2/TER/code/external/glfw-3.1.2/examples /home/e20190004794/Bureau/master_imagine/s2/TER/code/build /home/e20190004794/Bureau/master_imagine/s2/TER/code/build/external/glfw-3.1.2/examples /home/e20190004794/Bureau/master_imagine/s2/TER/code/build/external/glfw-3.1.2/examples/CMakeFiles/gears.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : external/glfw-3.1.2/examples/CMakeFiles/gears.dir/depend

