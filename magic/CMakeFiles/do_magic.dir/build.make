# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.21

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
CMAKE_SOURCE_DIR = /home/user/University/LinuxHW/magic

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/user/University/LinuxHW/magic

# Include any dependencies generated for this target.
include CMakeFiles/do_magic.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/do_magic.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/do_magic.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/do_magic.dir/flags.make

CMakeFiles/do_magic.dir/do_magic.cpp.o: CMakeFiles/do_magic.dir/flags.make
CMakeFiles/do_magic.dir/do_magic.cpp.o: do_magic.cpp
CMakeFiles/do_magic.dir/do_magic.cpp.o: CMakeFiles/do_magic.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/user/University/LinuxHW/magic/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/do_magic.dir/do_magic.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/do_magic.dir/do_magic.cpp.o -MF CMakeFiles/do_magic.dir/do_magic.cpp.o.d -o CMakeFiles/do_magic.dir/do_magic.cpp.o -c /home/user/University/LinuxHW/magic/do_magic.cpp

CMakeFiles/do_magic.dir/do_magic.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/do_magic.dir/do_magic.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/user/University/LinuxHW/magic/do_magic.cpp > CMakeFiles/do_magic.dir/do_magic.cpp.i

CMakeFiles/do_magic.dir/do_magic.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/do_magic.dir/do_magic.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/user/University/LinuxHW/magic/do_magic.cpp -o CMakeFiles/do_magic.dir/do_magic.cpp.s

# Object files for target do_magic
do_magic_OBJECTS = \
"CMakeFiles/do_magic.dir/do_magic.cpp.o"

# External object files for target do_magic
do_magic_EXTERNAL_OBJECTS =

do_magic: CMakeFiles/do_magic.dir/do_magic.cpp.o
do_magic: CMakeFiles/do_magic.dir/build.make
do_magic: CMakeFiles/do_magic.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/user/University/LinuxHW/magic/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable do_magic"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/do_magic.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/do_magic.dir/build: do_magic
.PHONY : CMakeFiles/do_magic.dir/build

CMakeFiles/do_magic.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/do_magic.dir/cmake_clean.cmake
.PHONY : CMakeFiles/do_magic.dir/clean

CMakeFiles/do_magic.dir/depend:
	cd /home/user/University/LinuxHW/magic && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/user/University/LinuxHW/magic /home/user/University/LinuxHW/magic /home/user/University/LinuxHW/magic /home/user/University/LinuxHW/magic /home/user/University/LinuxHW/magic/CMakeFiles/do_magic.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/do_magic.dir/depend

