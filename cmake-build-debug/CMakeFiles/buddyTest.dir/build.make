# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.19

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

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\JetBrains\CLion 2021.1.2\bin\cmake\win\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\JetBrains\CLion 2021.1.2\bin\cmake\win\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = C:\Users\50633\CLionProjects\BuddySystem

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = C:\Users\50633\CLionProjects\BuddySystem\cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/buddyTest.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/buddyTest.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/buddyTest.dir/flags.make

CMakeFiles/buddyTest.dir/buddySystem/buddyTest.c.obj: CMakeFiles/buddyTest.dir/flags.make
CMakeFiles/buddyTest.dir/buddySystem/buddyTest.c.obj: CMakeFiles/buddyTest.dir/includes_C.rsp
CMakeFiles/buddyTest.dir/buddySystem/buddyTest.c.obj: ../buddySystem/buddyTest.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\50633\CLionProjects\BuddySystem\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/buddyTest.dir/buddySystem/buddyTest.c.obj"
	C:\PROGRA~1\MinGW\X86_64~1.0-R\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles\buddyTest.dir\buddySystem\buddyTest.c.obj -c C:\Users\50633\CLionProjects\BuddySystem\buddySystem\buddyTest.c

CMakeFiles/buddyTest.dir/buddySystem/buddyTest.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/buddyTest.dir/buddySystem/buddyTest.c.i"
	C:\PROGRA~1\MinGW\X86_64~1.0-R\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E C:\Users\50633\CLionProjects\BuddySystem\buddySystem\buddyTest.c > CMakeFiles\buddyTest.dir\buddySystem\buddyTest.c.i

CMakeFiles/buddyTest.dir/buddySystem/buddyTest.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/buddyTest.dir/buddySystem/buddyTest.c.s"
	C:\PROGRA~1\MinGW\X86_64~1.0-R\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S C:\Users\50633\CLionProjects\BuddySystem\buddySystem\buddyTest.c -o CMakeFiles\buddyTest.dir\buddySystem\buddyTest.c.s

# Object files for target buddyTest
buddyTest_OBJECTS = \
"CMakeFiles/buddyTest.dir/buddySystem/buddyTest.c.obj"

# External object files for target buddyTest
buddyTest_EXTERNAL_OBJECTS =

buddyTest.exe: CMakeFiles/buddyTest.dir/buddySystem/buddyTest.c.obj
buddyTest.exe: CMakeFiles/buddyTest.dir/build.make
buddyTest.exe: CMakeFiles/buddyTest.dir/linklibs.rsp
buddyTest.exe: CMakeFiles/buddyTest.dir/objects1.rsp
buddyTest.exe: CMakeFiles/buddyTest.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=C:\Users\50633\CLionProjects\BuddySystem\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable buddyTest.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\buddyTest.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/buddyTest.dir/build: buddyTest.exe

.PHONY : CMakeFiles/buddyTest.dir/build

CMakeFiles/buddyTest.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\buddyTest.dir\cmake_clean.cmake
.PHONY : CMakeFiles/buddyTest.dir/clean

CMakeFiles/buddyTest.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" C:\Users\50633\CLionProjects\BuddySystem C:\Users\50633\CLionProjects\BuddySystem C:\Users\50633\CLionProjects\BuddySystem\cmake-build-debug C:\Users\50633\CLionProjects\BuddySystem\cmake-build-debug C:\Users\50633\CLionProjects\BuddySystem\cmake-build-debug\CMakeFiles\buddyTest.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/buddyTest.dir/depend

