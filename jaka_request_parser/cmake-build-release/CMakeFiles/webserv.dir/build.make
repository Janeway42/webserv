# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.24

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
CMAKE_COMMAND = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake

# The command to remove a file.
RM = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/joycemacksuele/42/RANKS/rank05/webserv

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/joycemacksuele/42/RANKS/rank05/webserv/cmake-build-release

# Include any dependencies generated for this target.
include CMakeFiles/webserv.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/webserv.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/webserv.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/webserv.dir/flags.make

CMakeFiles/webserv.dir/srcs/ConfigFileParser.cpp.o: CMakeFiles/webserv.dir/flags.make
CMakeFiles/webserv.dir/srcs/ConfigFileParser.cpp.o: /Users/joycemacksuele/42/RANKS/rank05/webserv/srcs/ConfigFileParser.cpp
CMakeFiles/webserv.dir/srcs/ConfigFileParser.cpp.o: CMakeFiles/webserv.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/joycemacksuele/42/RANKS/rank05/webserv/cmake-build-release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/webserv.dir/srcs/ConfigFileParser.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/webserv.dir/srcs/ConfigFileParser.cpp.o -MF CMakeFiles/webserv.dir/srcs/ConfigFileParser.cpp.o.d -o CMakeFiles/webserv.dir/srcs/ConfigFileParser.cpp.o -c /Users/joycemacksuele/42/RANKS/rank05/webserv/srcs/ConfigFileParser.cpp

CMakeFiles/webserv.dir/srcs/ConfigFileParser.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/webserv.dir/srcs/ConfigFileParser.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/joycemacksuele/42/RANKS/rank05/webserv/srcs/ConfigFileParser.cpp > CMakeFiles/webserv.dir/srcs/ConfigFileParser.cpp.i

CMakeFiles/webserv.dir/srcs/ConfigFileParser.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/webserv.dir/srcs/ConfigFileParser.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/joycemacksuele/42/RANKS/rank05/webserv/srcs/ConfigFileParser.cpp -o CMakeFiles/webserv.dir/srcs/ConfigFileParser.cpp.s

CMakeFiles/webserv.dir/srcs/main.cpp.o: CMakeFiles/webserv.dir/flags.make
CMakeFiles/webserv.dir/srcs/main.cpp.o: /Users/joycemacksuele/42/RANKS/rank05/webserv/srcs/main.cpp
CMakeFiles/webserv.dir/srcs/main.cpp.o: CMakeFiles/webserv.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/joycemacksuele/42/RANKS/rank05/webserv/cmake-build-release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/webserv.dir/srcs/main.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/webserv.dir/srcs/main.cpp.o -MF CMakeFiles/webserv.dir/srcs/main.cpp.o.d -o CMakeFiles/webserv.dir/srcs/main.cpp.o -c /Users/joycemacksuele/42/RANKS/rank05/webserv/srcs/main.cpp

CMakeFiles/webserv.dir/srcs/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/webserv.dir/srcs/main.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/joycemacksuele/42/RANKS/rank05/webserv/srcs/main.cpp > CMakeFiles/webserv.dir/srcs/main.cpp.i

CMakeFiles/webserv.dir/srcs/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/webserv.dir/srcs/main.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/joycemacksuele/42/RANKS/rank05/webserv/srcs/main.cpp -o CMakeFiles/webserv.dir/srcs/main.cpp.s

CMakeFiles/webserv.dir/srcs/Parser.cpp.o: CMakeFiles/webserv.dir/flags.make
CMakeFiles/webserv.dir/srcs/Parser.cpp.o: /Users/joycemacksuele/42/RANKS/rank05/webserv/srcs/Parser.cpp
CMakeFiles/webserv.dir/srcs/Parser.cpp.o: CMakeFiles/webserv.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/joycemacksuele/42/RANKS/rank05/webserv/cmake-build-release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/webserv.dir/srcs/Parser.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/webserv.dir/srcs/Parser.cpp.o -MF CMakeFiles/webserv.dir/srcs/Parser.cpp.o.d -o CMakeFiles/webserv.dir/srcs/Parser.cpp.o -c /Users/joycemacksuele/42/RANKS/rank05/webserv/srcs/Parser.cpp

CMakeFiles/webserv.dir/srcs/Parser.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/webserv.dir/srcs/Parser.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/joycemacksuele/42/RANKS/rank05/webserv/srcs/Parser.cpp > CMakeFiles/webserv.dir/srcs/Parser.cpp.i

CMakeFiles/webserv.dir/srcs/Parser.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/webserv.dir/srcs/Parser.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/joycemacksuele/42/RANKS/rank05/webserv/srcs/Parser.cpp -o CMakeFiles/webserv.dir/srcs/Parser.cpp.s

CMakeFiles/webserv.dir/srcs/ServerData.cpp.o: CMakeFiles/webserv.dir/flags.make
CMakeFiles/webserv.dir/srcs/ServerData.cpp.o: /Users/joycemacksuele/42/RANKS/rank05/webserv/srcs/ServerData.cpp
CMakeFiles/webserv.dir/srcs/ServerData.cpp.o: CMakeFiles/webserv.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/joycemacksuele/42/RANKS/rank05/webserv/cmake-build-release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/webserv.dir/srcs/ServerData.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/webserv.dir/srcs/ServerData.cpp.o -MF CMakeFiles/webserv.dir/srcs/ServerData.cpp.o.d -o CMakeFiles/webserv.dir/srcs/ServerData.cpp.o -c /Users/joycemacksuele/42/RANKS/rank05/webserv/srcs/ServerData.cpp

CMakeFiles/webserv.dir/srcs/ServerData.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/webserv.dir/srcs/ServerData.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/joycemacksuele/42/RANKS/rank05/webserv/srcs/ServerData.cpp > CMakeFiles/webserv.dir/srcs/ServerData.cpp.i

CMakeFiles/webserv.dir/srcs/ServerData.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/webserv.dir/srcs/ServerData.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/joycemacksuele/42/RANKS/rank05/webserv/srcs/ServerData.cpp -o CMakeFiles/webserv.dir/srcs/ServerData.cpp.s

# Object files for target webserv
webserv_OBJECTS = \
"CMakeFiles/webserv.dir/srcs/ConfigFileParser.cpp.o" \
"CMakeFiles/webserv.dir/srcs/main.cpp.o" \
"CMakeFiles/webserv.dir/srcs/Parser.cpp.o" \
"CMakeFiles/webserv.dir/srcs/ServerData.cpp.o"

# External object files for target webserv
webserv_EXTERNAL_OBJECTS =

webserv: CMakeFiles/webserv.dir/srcs/ConfigFileParser.cpp.o
webserv: CMakeFiles/webserv.dir/srcs/main.cpp.o
webserv: CMakeFiles/webserv.dir/srcs/Parser.cpp.o
webserv: CMakeFiles/webserv.dir/srcs/ServerData.cpp.o
webserv: CMakeFiles/webserv.dir/build.make
webserv: CMakeFiles/webserv.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/joycemacksuele/42/RANKS/rank05/webserv/cmake-build-release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Linking CXX executable webserv"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/webserv.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/webserv.dir/build: webserv
.PHONY : CMakeFiles/webserv.dir/build

CMakeFiles/webserv.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/webserv.dir/cmake_clean.cmake
.PHONY : CMakeFiles/webserv.dir/clean

CMakeFiles/webserv.dir/depend:
	cd /Users/joycemacksuele/42/RANKS/rank05/webserv/cmake-build-release && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/joycemacksuele/42/RANKS/rank05/webserv /Users/joycemacksuele/42/RANKS/rank05/webserv /Users/joycemacksuele/42/RANKS/rank05/webserv/cmake-build-release /Users/joycemacksuele/42/RANKS/rank05/webserv/cmake-build-release /Users/joycemacksuele/42/RANKS/rank05/webserv/cmake-build-release/CMakeFiles/webserv.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/webserv.dir/depend
