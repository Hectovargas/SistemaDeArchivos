# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.31

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
CMAKE_SOURCE_DIR = "/home/fefo/Documentos/GitHub/SistemaDeArchivos/Proyeto blockDevice"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "/home/fefo/Documentos/GitHub/SistemaDeArchivos/Proyeto blockDevice/build"

# Include any dependencies generated for this target.
include CMakeFiles/blockdevice.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/blockdevice.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/blockdevice.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/blockdevice.dir/flags.make

CMakeFiles/blockdevice.dir/codegen:
.PHONY : CMakeFiles/blockdevice.dir/codegen

CMakeFiles/blockdevice.dir/filesystem.cpp.o: CMakeFiles/blockdevice.dir/flags.make
CMakeFiles/blockdevice.dir/filesystem.cpp.o: /home/fefo/Documentos/GitHub/SistemaDeArchivos/Proyeto\ blockDevice/filesystem.cpp
CMakeFiles/blockdevice.dir/filesystem.cpp.o: CMakeFiles/blockdevice.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir="/home/fefo/Documentos/GitHub/SistemaDeArchivos/Proyeto blockDevice/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/blockdevice.dir/filesystem.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/blockdevice.dir/filesystem.cpp.o -MF CMakeFiles/blockdevice.dir/filesystem.cpp.o.d -o CMakeFiles/blockdevice.dir/filesystem.cpp.o -c "/home/fefo/Documentos/GitHub/SistemaDeArchivos/Proyeto blockDevice/filesystem.cpp"

CMakeFiles/blockdevice.dir/filesystem.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/blockdevice.dir/filesystem.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/home/fefo/Documentos/GitHub/SistemaDeArchivos/Proyeto blockDevice/filesystem.cpp" > CMakeFiles/blockdevice.dir/filesystem.cpp.i

CMakeFiles/blockdevice.dir/filesystem.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/blockdevice.dir/filesystem.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/home/fefo/Documentos/GitHub/SistemaDeArchivos/Proyeto blockDevice/filesystem.cpp" -o CMakeFiles/blockdevice.dir/filesystem.cpp.s

CMakeFiles/blockdevice.dir/Interfaz.cpp.o: CMakeFiles/blockdevice.dir/flags.make
CMakeFiles/blockdevice.dir/Interfaz.cpp.o: /home/fefo/Documentos/GitHub/SistemaDeArchivos/Proyeto\ blockDevice/Interfaz.cpp
CMakeFiles/blockdevice.dir/Interfaz.cpp.o: CMakeFiles/blockdevice.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir="/home/fefo/Documentos/GitHub/SistemaDeArchivos/Proyeto blockDevice/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/blockdevice.dir/Interfaz.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/blockdevice.dir/Interfaz.cpp.o -MF CMakeFiles/blockdevice.dir/Interfaz.cpp.o.d -o CMakeFiles/blockdevice.dir/Interfaz.cpp.o -c "/home/fefo/Documentos/GitHub/SistemaDeArchivos/Proyeto blockDevice/Interfaz.cpp"

CMakeFiles/blockdevice.dir/Interfaz.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/blockdevice.dir/Interfaz.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/home/fefo/Documentos/GitHub/SistemaDeArchivos/Proyeto blockDevice/Interfaz.cpp" > CMakeFiles/blockdevice.dir/Interfaz.cpp.i

CMakeFiles/blockdevice.dir/Interfaz.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/blockdevice.dir/Interfaz.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/home/fefo/Documentos/GitHub/SistemaDeArchivos/Proyeto blockDevice/Interfaz.cpp" -o CMakeFiles/blockdevice.dir/Interfaz.cpp.s

# Object files for target blockdevice
blockdevice_OBJECTS = \
"CMakeFiles/blockdevice.dir/filesystem.cpp.o" \
"CMakeFiles/blockdevice.dir/Interfaz.cpp.o"

# External object files for target blockdevice
blockdevice_EXTERNAL_OBJECTS =

blockdevice: CMakeFiles/blockdevice.dir/filesystem.cpp.o
blockdevice: CMakeFiles/blockdevice.dir/Interfaz.cpp.o
blockdevice: CMakeFiles/blockdevice.dir/build.make
blockdevice: CMakeFiles/blockdevice.dir/compiler_depend.ts
blockdevice: CMakeFiles/blockdevice.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir="/home/fefo/Documentos/GitHub/SistemaDeArchivos/Proyeto blockDevice/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable blockdevice"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/blockdevice.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/blockdevice.dir/build: blockdevice
.PHONY : CMakeFiles/blockdevice.dir/build

CMakeFiles/blockdevice.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/blockdevice.dir/cmake_clean.cmake
.PHONY : CMakeFiles/blockdevice.dir/clean

CMakeFiles/blockdevice.dir/depend:
	cd "/home/fefo/Documentos/GitHub/SistemaDeArchivos/Proyeto blockDevice/build" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/home/fefo/Documentos/GitHub/SistemaDeArchivos/Proyeto blockDevice" "/home/fefo/Documentos/GitHub/SistemaDeArchivos/Proyeto blockDevice" "/home/fefo/Documentos/GitHub/SistemaDeArchivos/Proyeto blockDevice/build" "/home/fefo/Documentos/GitHub/SistemaDeArchivos/Proyeto blockDevice/build" "/home/fefo/Documentos/GitHub/SistemaDeArchivos/Proyeto blockDevice/build/CMakeFiles/blockdevice.dir/DependInfo.cmake" "--color=$(COLOR)"
.PHONY : CMakeFiles/blockdevice.dir/depend

