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
CMAKE_SOURCE_DIR = /home/mars/code/m_websever

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/mars/code/m_websever/build

# Include any dependencies generated for this target.
include CMakeFiles/http_server.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/http_server.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/http_server.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/http_server.dir/flags.make

CMakeFiles/http_server.dir/http_server.cc.o: CMakeFiles/http_server.dir/flags.make
CMakeFiles/http_server.dir/http_server.cc.o: /home/mars/code/m_websever/http_server.cc
CMakeFiles/http_server.dir/http_server.cc.o: CMakeFiles/http_server.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/mars/code/m_websever/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/http_server.dir/http_server.cc.o"
	clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/http_server.dir/http_server.cc.o -MF CMakeFiles/http_server.dir/http_server.cc.o.d -o CMakeFiles/http_server.dir/http_server.cc.o -c /home/mars/code/m_websever/http_server.cc

CMakeFiles/http_server.dir/http_server.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/http_server.dir/http_server.cc.i"
	clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/mars/code/m_websever/http_server.cc > CMakeFiles/http_server.dir/http_server.cc.i

CMakeFiles/http_server.dir/http_server.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/http_server.dir/http_server.cc.s"
	clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/mars/code/m_websever/http_server.cc -o CMakeFiles/http_server.dir/http_server.cc.s

CMakeFiles/http_server.dir/http_context.cc.o: CMakeFiles/http_server.dir/flags.make
CMakeFiles/http_server.dir/http_context.cc.o: /home/mars/code/m_websever/http_context.cc
CMakeFiles/http_server.dir/http_context.cc.o: CMakeFiles/http_server.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/mars/code/m_websever/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/http_server.dir/http_context.cc.o"
	clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/http_server.dir/http_context.cc.o -MF CMakeFiles/http_server.dir/http_context.cc.o.d -o CMakeFiles/http_server.dir/http_context.cc.o -c /home/mars/code/m_websever/http_context.cc

CMakeFiles/http_server.dir/http_context.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/http_server.dir/http_context.cc.i"
	clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/mars/code/m_websever/http_context.cc > CMakeFiles/http_server.dir/http_context.cc.i

CMakeFiles/http_server.dir/http_context.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/http_server.dir/http_context.cc.s"
	clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/mars/code/m_websever/http_context.cc -o CMakeFiles/http_server.dir/http_context.cc.s

CMakeFiles/http_server.dir/http_response.cc.o: CMakeFiles/http_server.dir/flags.make
CMakeFiles/http_server.dir/http_response.cc.o: /home/mars/code/m_websever/http_response.cc
CMakeFiles/http_server.dir/http_response.cc.o: CMakeFiles/http_server.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/mars/code/m_websever/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/http_server.dir/http_response.cc.o"
	clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/http_server.dir/http_response.cc.o -MF CMakeFiles/http_server.dir/http_response.cc.o.d -o CMakeFiles/http_server.dir/http_response.cc.o -c /home/mars/code/m_websever/http_response.cc

CMakeFiles/http_server.dir/http_response.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/http_server.dir/http_response.cc.i"
	clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/mars/code/m_websever/http_response.cc > CMakeFiles/http_server.dir/http_response.cc.i

CMakeFiles/http_server.dir/http_response.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/http_server.dir/http_response.cc.s"
	clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/mars/code/m_websever/http_response.cc -o CMakeFiles/http_server.dir/http_response.cc.s

CMakeFiles/http_server.dir/main.cc.o: CMakeFiles/http_server.dir/flags.make
CMakeFiles/http_server.dir/main.cc.o: /home/mars/code/m_websever/main.cc
CMakeFiles/http_server.dir/main.cc.o: CMakeFiles/http_server.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/mars/code/m_websever/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/http_server.dir/main.cc.o"
	clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/http_server.dir/main.cc.o -MF CMakeFiles/http_server.dir/main.cc.o.d -o CMakeFiles/http_server.dir/main.cc.o -c /home/mars/code/m_websever/main.cc

CMakeFiles/http_server.dir/main.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/http_server.dir/main.cc.i"
	clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/mars/code/m_websever/main.cc > CMakeFiles/http_server.dir/main.cc.i

CMakeFiles/http_server.dir/main.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/http_server.dir/main.cc.s"
	clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/mars/code/m_websever/main.cc -o CMakeFiles/http_server.dir/main.cc.s

# Object files for target http_server
http_server_OBJECTS = \
"CMakeFiles/http_server.dir/http_server.cc.o" \
"CMakeFiles/http_server.dir/http_context.cc.o" \
"CMakeFiles/http_server.dir/http_response.cc.o" \
"CMakeFiles/http_server.dir/main.cc.o"

# External object files for target http_server
http_server_EXTERNAL_OBJECTS =

http_server: CMakeFiles/http_server.dir/http_server.cc.o
http_server: CMakeFiles/http_server.dir/http_context.cc.o
http_server: CMakeFiles/http_server.dir/http_response.cc.o
http_server: CMakeFiles/http_server.dir/main.cc.o
http_server: CMakeFiles/http_server.dir/build.make
http_server: m_netlib/libm_netlib.a
http_server: CMakeFiles/http_server.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/mars/code/m_websever/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Linking CXX executable http_server"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/http_server.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/http_server.dir/build: http_server
.PHONY : CMakeFiles/http_server.dir/build

CMakeFiles/http_server.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/http_server.dir/cmake_clean.cmake
.PHONY : CMakeFiles/http_server.dir/clean

CMakeFiles/http_server.dir/depend:
	cd /home/mars/code/m_websever/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/mars/code/m_websever /home/mars/code/m_websever /home/mars/code/m_websever/build /home/mars/code/m_websever/build /home/mars/code/m_websever/build/CMakeFiles/http_server.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/http_server.dir/depend
