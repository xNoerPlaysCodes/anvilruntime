# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.30

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
CMAKE_SOURCE_DIR = /home/noerlol/C-Projects/AnvilRuntime

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/noerlol/C-Projects/AnvilRuntime

# Include any dependencies generated for this target.
include CMakeFiles/anvilruntime_test.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/anvilruntime_test.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/anvilruntime_test.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/anvilruntime_test.dir/flags.make

CMakeFiles/anvilruntime_test.dir/tests/anvilruntime_test.cpp.o: CMakeFiles/anvilruntime_test.dir/flags.make
CMakeFiles/anvilruntime_test.dir/tests/anvilruntime_test.cpp.o: tests/anvilruntime_test.cpp
CMakeFiles/anvilruntime_test.dir/tests/anvilruntime_test.cpp.o: CMakeFiles/anvilruntime_test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/noerlol/C-Projects/AnvilRuntime/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/anvilruntime_test.dir/tests/anvilruntime_test.cpp.o"
	/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/anvilruntime_test.dir/tests/anvilruntime_test.cpp.o -MF CMakeFiles/anvilruntime_test.dir/tests/anvilruntime_test.cpp.o.d -o CMakeFiles/anvilruntime_test.dir/tests/anvilruntime_test.cpp.o -c /home/noerlol/C-Projects/AnvilRuntime/tests/anvilruntime_test.cpp

CMakeFiles/anvilruntime_test.dir/tests/anvilruntime_test.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/anvilruntime_test.dir/tests/anvilruntime_test.cpp.i"
	/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/noerlol/C-Projects/AnvilRuntime/tests/anvilruntime_test.cpp > CMakeFiles/anvilruntime_test.dir/tests/anvilruntime_test.cpp.i

CMakeFiles/anvilruntime_test.dir/tests/anvilruntime_test.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/anvilruntime_test.dir/tests/anvilruntime_test.cpp.s"
	/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/noerlol/C-Projects/AnvilRuntime/tests/anvilruntime_test.cpp -o CMakeFiles/anvilruntime_test.dir/tests/anvilruntime_test.cpp.s

# Object files for target anvilruntime_test
anvilruntime_test_OBJECTS = \
"CMakeFiles/anvilruntime_test.dir/tests/anvilruntime_test.cpp.o"

# External object files for target anvilruntime_test
anvilruntime_test_EXTERNAL_OBJECTS =

bin/anvilruntime_test: CMakeFiles/anvilruntime_test.dir/tests/anvilruntime_test.cpp.o
bin/anvilruntime_test: CMakeFiles/anvilruntime_test.dir/build.make
bin/anvilruntime_test: bin/libanvilruntime.a
bin/anvilruntime_test: CMakeFiles/anvilruntime_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/noerlol/C-Projects/AnvilRuntime/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable bin/anvilruntime_test"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/anvilruntime_test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/anvilruntime_test.dir/build: bin/anvilruntime_test
.PHONY : CMakeFiles/anvilruntime_test.dir/build

CMakeFiles/anvilruntime_test.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/anvilruntime_test.dir/cmake_clean.cmake
.PHONY : CMakeFiles/anvilruntime_test.dir/clean

CMakeFiles/anvilruntime_test.dir/depend:
	cd /home/noerlol/C-Projects/AnvilRuntime && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/noerlol/C-Projects/AnvilRuntime /home/noerlol/C-Projects/AnvilRuntime /home/noerlol/C-Projects/AnvilRuntime /home/noerlol/C-Projects/AnvilRuntime /home/noerlol/C-Projects/AnvilRuntime/CMakeFiles/anvilruntime_test.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/anvilruntime_test.dir/depend
