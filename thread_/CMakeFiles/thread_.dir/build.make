# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.25

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
CMAKE_COMMAND = /home/dcao/download/miniconda3/envs/py310/lib/python3.10/site-packages/cmake/data/bin/cmake

# The command to remove a file.
RM = /home/dcao/download/miniconda3/envs/py310/lib/python3.10/site-packages/cmake/data/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/dcao/webserver/myServer

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/dcao/webserver/myServer/build

# Include any dependencies generated for this target.
include thread_/CMakeFiles/thread_.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include thread_/CMakeFiles/thread_.dir/compiler_depend.make

# Include the progress variables for this target.
include thread_/CMakeFiles/thread_.dir/progress.make

# Include the compile flags for this target's objects.
include thread_/CMakeFiles/thread_.dir/flags.make

thread_/CMakeFiles/thread_.dir/src/test1.cpp.o: thread_/CMakeFiles/thread_.dir/flags.make
thread_/CMakeFiles/thread_.dir/src/test1.cpp.o: /home/dcao/webserver/myServer/thread_/src/test1.cpp
thread_/CMakeFiles/thread_.dir/src/test1.cpp.o: thread_/CMakeFiles/thread_.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/dcao/webserver/myServer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object thread_/CMakeFiles/thread_.dir/src/test1.cpp.o"
	cd /home/dcao/webserver/myServer/build/thread_ && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT thread_/CMakeFiles/thread_.dir/src/test1.cpp.o -MF CMakeFiles/thread_.dir/src/test1.cpp.o.d -o CMakeFiles/thread_.dir/src/test1.cpp.o -c /home/dcao/webserver/myServer/thread_/src/test1.cpp

thread_/CMakeFiles/thread_.dir/src/test1.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/thread_.dir/src/test1.cpp.i"
	cd /home/dcao/webserver/myServer/build/thread_ && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/dcao/webserver/myServer/thread_/src/test1.cpp > CMakeFiles/thread_.dir/src/test1.cpp.i

thread_/CMakeFiles/thread_.dir/src/test1.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/thread_.dir/src/test1.cpp.s"
	cd /home/dcao/webserver/myServer/build/thread_ && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/dcao/webserver/myServer/thread_/src/test1.cpp -o CMakeFiles/thread_.dir/src/test1.cpp.s

# Object files for target thread_
thread__OBJECTS = \
"CMakeFiles/thread_.dir/src/test1.cpp.o"

# External object files for target thread_
thread__EXTERNAL_OBJECTS =

thread_/libthread_.a: thread_/CMakeFiles/thread_.dir/src/test1.cpp.o
thread_/libthread_.a: thread_/CMakeFiles/thread_.dir/build.make
thread_/libthread_.a: thread_/CMakeFiles/thread_.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/dcao/webserver/myServer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libthread_.a"
	cd /home/dcao/webserver/myServer/build/thread_ && $(CMAKE_COMMAND) -P CMakeFiles/thread_.dir/cmake_clean_target.cmake
	cd /home/dcao/webserver/myServer/build/thread_ && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/thread_.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
thread_/CMakeFiles/thread_.dir/build: thread_/libthread_.a
.PHONY : thread_/CMakeFiles/thread_.dir/build

thread_/CMakeFiles/thread_.dir/clean:
	cd /home/dcao/webserver/myServer/build/thread_ && $(CMAKE_COMMAND) -P CMakeFiles/thread_.dir/cmake_clean.cmake
.PHONY : thread_/CMakeFiles/thread_.dir/clean

thread_/CMakeFiles/thread_.dir/depend:
	cd /home/dcao/webserver/myServer/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/dcao/webserver/myServer /home/dcao/webserver/myServer/thread_ /home/dcao/webserver/myServer/build /home/dcao/webserver/myServer/build/thread_ /home/dcao/webserver/myServer/build/thread_/CMakeFiles/thread_.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : thread_/CMakeFiles/thread_.dir/depend

