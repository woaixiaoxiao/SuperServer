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
include server_/CMakeFiles/server_.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include server_/CMakeFiles/server_.dir/compiler_depend.make

# Include the progress variables for this target.
include server_/CMakeFiles/server_.dir/progress.make

# Include the compile flags for this target's objects.
include server_/CMakeFiles/server_.dir/flags.make

server_/CMakeFiles/server_.dir/src/server.cpp.o: server_/CMakeFiles/server_.dir/flags.make
server_/CMakeFiles/server_.dir/src/server.cpp.o: /home/dcao/webserver/myServer/server_/src/server.cpp
server_/CMakeFiles/server_.dir/src/server.cpp.o: server_/CMakeFiles/server_.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/dcao/webserver/myServer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object server_/CMakeFiles/server_.dir/src/server.cpp.o"
	cd /home/dcao/webserver/myServer/build/server_ && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT server_/CMakeFiles/server_.dir/src/server.cpp.o -MF CMakeFiles/server_.dir/src/server.cpp.o.d -o CMakeFiles/server_.dir/src/server.cpp.o -c /home/dcao/webserver/myServer/server_/src/server.cpp

server_/CMakeFiles/server_.dir/src/server.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/server_.dir/src/server.cpp.i"
	cd /home/dcao/webserver/myServer/build/server_ && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/dcao/webserver/myServer/server_/src/server.cpp > CMakeFiles/server_.dir/src/server.cpp.i

server_/CMakeFiles/server_.dir/src/server.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/server_.dir/src/server.cpp.s"
	cd /home/dcao/webserver/myServer/build/server_ && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/dcao/webserver/myServer/server_/src/server.cpp -o CMakeFiles/server_.dir/src/server.cpp.s

# Object files for target server_
server__OBJECTS = \
"CMakeFiles/server_.dir/src/server.cpp.o"

# External object files for target server_
server__EXTERNAL_OBJECTS =

server_/server_: server_/CMakeFiles/server_.dir/src/server.cpp.o
server_/server_: server_/CMakeFiles/server_.dir/build.make
server_/server_: thread_/libthread_.a
server_/server_: server_/CMakeFiles/server_.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/dcao/webserver/myServer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable server_"
	cd /home/dcao/webserver/myServer/build/server_ && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/server_.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
server_/CMakeFiles/server_.dir/build: server_/server_
.PHONY : server_/CMakeFiles/server_.dir/build

server_/CMakeFiles/server_.dir/clean:
	cd /home/dcao/webserver/myServer/build/server_ && $(CMAKE_COMMAND) -P CMakeFiles/server_.dir/cmake_clean.cmake
.PHONY : server_/CMakeFiles/server_.dir/clean

server_/CMakeFiles/server_.dir/depend:
	cd /home/dcao/webserver/myServer/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/dcao/webserver/myServer /home/dcao/webserver/myServer/server_ /home/dcao/webserver/myServer/build /home/dcao/webserver/myServer/build/server_ /home/dcao/webserver/myServer/build/server_/CMakeFiles/server_.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : server_/CMakeFiles/server_.dir/depend
