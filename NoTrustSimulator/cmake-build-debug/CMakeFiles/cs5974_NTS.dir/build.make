# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.8

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

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\JetBrains\CLion 2017.2.2\bin\cmake\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\JetBrains\CLion 2017.2.2\bin\cmake\bin\cmake.exe" -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = C:\Users\kdkarki\Documents\workspace\github\CS5974\NoTrustScore

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = C:\Users\kdkarki\Documents\workspace\github\CS5974\NoTrustScore\cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/cs5974_NTS.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/cs5974_NTS.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/cs5974_NTS.dir/flags.make

CMakeFiles/cs5974_NTS.dir/bmeans.c.obj: CMakeFiles/cs5974_NTS.dir/flags.make
CMakeFiles/cs5974_NTS.dir/bmeans.c.obj: ../bmeans.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\kdkarki\Documents\workspace\github\CS5974\NoTrustScore\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/cs5974_NTS.dir/bmeans.c.obj"
	C:\PROGRA~2\CODEBL~1\MinGW\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles\cs5974_NTS.dir\bmeans.c.obj   -c C:\Users\kdkarki\Documents\workspace\github\CS5974\NoTrustScore\bmeans.c

CMakeFiles/cs5974_NTS.dir/bmeans.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/cs5974_NTS.dir/bmeans.c.i"
	C:\PROGRA~2\CODEBL~1\MinGW\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E C:\Users\kdkarki\Documents\workspace\github\CS5974\NoTrustScore\bmeans.c > CMakeFiles\cs5974_NTS.dir\bmeans.c.i

CMakeFiles/cs5974_NTS.dir/bmeans.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/cs5974_NTS.dir/bmeans.c.s"
	C:\PROGRA~2\CODEBL~1\MinGW\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S C:\Users\kdkarki\Documents\workspace\github\CS5974\NoTrustScore\bmeans.c -o CMakeFiles\cs5974_NTS.dir\bmeans.c.s

CMakeFiles/cs5974_NTS.dir/bmeans.c.obj.requires:

.PHONY : CMakeFiles/cs5974_NTS.dir/bmeans.c.obj.requires

CMakeFiles/cs5974_NTS.dir/bmeans.c.obj.provides: CMakeFiles/cs5974_NTS.dir/bmeans.c.obj.requires
	$(MAKE) -f CMakeFiles\cs5974_NTS.dir\build.make CMakeFiles/cs5974_NTS.dir/bmeans.c.obj.provides.build
.PHONY : CMakeFiles/cs5974_NTS.dir/bmeans.c.obj.provides

CMakeFiles/cs5974_NTS.dir/bmeans.c.obj.provides.build: CMakeFiles/cs5974_NTS.dir/bmeans.c.obj


CMakeFiles/cs5974_NTS.dir/smpl.c.obj: CMakeFiles/cs5974_NTS.dir/flags.make
CMakeFiles/cs5974_NTS.dir/smpl.c.obj: ../smpl.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\kdkarki\Documents\workspace\github\CS5974\NoTrustScore\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/cs5974_NTS.dir/smpl.c.obj"
	C:\PROGRA~2\CODEBL~1\MinGW\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles\cs5974_NTS.dir\smpl.c.obj   -c C:\Users\kdkarki\Documents\workspace\github\CS5974\NoTrustScore\smpl.c

CMakeFiles/cs5974_NTS.dir/smpl.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/cs5974_NTS.dir/smpl.c.i"
	C:\PROGRA~2\CODEBL~1\MinGW\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E C:\Users\kdkarki\Documents\workspace\github\CS5974\NoTrustScore\smpl.c > CMakeFiles\cs5974_NTS.dir\smpl.c.i

CMakeFiles/cs5974_NTS.dir/smpl.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/cs5974_NTS.dir/smpl.c.s"
	C:\PROGRA~2\CODEBL~1\MinGW\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S C:\Users\kdkarki\Documents\workspace\github\CS5974\NoTrustScore\smpl.c -o CMakeFiles\cs5974_NTS.dir\smpl.c.s

CMakeFiles/cs5974_NTS.dir/smpl.c.obj.requires:

.PHONY : CMakeFiles/cs5974_NTS.dir/smpl.c.obj.requires

CMakeFiles/cs5974_NTS.dir/smpl.c.obj.provides: CMakeFiles/cs5974_NTS.dir/smpl.c.obj.requires
	$(MAKE) -f CMakeFiles\cs5974_NTS.dir\build.make CMakeFiles/cs5974_NTS.dir/smpl.c.obj.provides.build
.PHONY : CMakeFiles/cs5974_NTS.dir/smpl.c.obj.provides

CMakeFiles/cs5974_NTS.dir/smpl.c.obj.provides.build: CMakeFiles/cs5974_NTS.dir/smpl.c.obj


CMakeFiles/cs5974_NTS.dir/stat.c.obj: CMakeFiles/cs5974_NTS.dir/flags.make
CMakeFiles/cs5974_NTS.dir/stat.c.obj: ../stat.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\kdkarki\Documents\workspace\github\CS5974\NoTrustScore\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object CMakeFiles/cs5974_NTS.dir/stat.c.obj"
	C:\PROGRA~2\CODEBL~1\MinGW\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles\cs5974_NTS.dir\stat.c.obj   -c C:\Users\kdkarki\Documents\workspace\github\CS5974\NoTrustScore\stat.c

CMakeFiles/cs5974_NTS.dir/stat.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/cs5974_NTS.dir/stat.c.i"
	C:\PROGRA~2\CODEBL~1\MinGW\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E C:\Users\kdkarki\Documents\workspace\github\CS5974\NoTrustScore\stat.c > CMakeFiles\cs5974_NTS.dir\stat.c.i

CMakeFiles/cs5974_NTS.dir/stat.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/cs5974_NTS.dir/stat.c.s"
	C:\PROGRA~2\CODEBL~1\MinGW\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S C:\Users\kdkarki\Documents\workspace\github\CS5974\NoTrustScore\stat.c -o CMakeFiles\cs5974_NTS.dir\stat.c.s

CMakeFiles/cs5974_NTS.dir/stat.c.obj.requires:

.PHONY : CMakeFiles/cs5974_NTS.dir/stat.c.obj.requires

CMakeFiles/cs5974_NTS.dir/stat.c.obj.provides: CMakeFiles/cs5974_NTS.dir/stat.c.obj.requires
	$(MAKE) -f CMakeFiles\cs5974_NTS.dir\build.make CMakeFiles/cs5974_NTS.dir/stat.c.obj.provides.build
.PHONY : CMakeFiles/cs5974_NTS.dir/stat.c.obj.provides

CMakeFiles/cs5974_NTS.dir/stat.c.obj.provides.build: CMakeFiles/cs5974_NTS.dir/stat.c.obj


CMakeFiles/cs5974_NTS.dir/rand.c.obj: CMakeFiles/cs5974_NTS.dir/flags.make
CMakeFiles/cs5974_NTS.dir/rand.c.obj: ../rand.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\kdkarki\Documents\workspace\github\CS5974\NoTrustScore\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object CMakeFiles/cs5974_NTS.dir/rand.c.obj"
	C:\PROGRA~2\CODEBL~1\MinGW\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles\cs5974_NTS.dir\rand.c.obj   -c C:\Users\kdkarki\Documents\workspace\github\CS5974\NoTrustScore\rand.c

CMakeFiles/cs5974_NTS.dir/rand.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/cs5974_NTS.dir/rand.c.i"
	C:\PROGRA~2\CODEBL~1\MinGW\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E C:\Users\kdkarki\Documents\workspace\github\CS5974\NoTrustScore\rand.c > CMakeFiles\cs5974_NTS.dir\rand.c.i

CMakeFiles/cs5974_NTS.dir/rand.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/cs5974_NTS.dir/rand.c.s"
	C:\PROGRA~2\CODEBL~1\MinGW\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S C:\Users\kdkarki\Documents\workspace\github\CS5974\NoTrustScore\rand.c -o CMakeFiles\cs5974_NTS.dir\rand.c.s

CMakeFiles/cs5974_NTS.dir/rand.c.obj.requires:

.PHONY : CMakeFiles/cs5974_NTS.dir/rand.c.obj.requires

CMakeFiles/cs5974_NTS.dir/rand.c.obj.provides: CMakeFiles/cs5974_NTS.dir/rand.c.obj.requires
	$(MAKE) -f CMakeFiles\cs5974_NTS.dir\build.make CMakeFiles/cs5974_NTS.dir/rand.c.obj.provides.build
.PHONY : CMakeFiles/cs5974_NTS.dir/rand.c.obj.provides

CMakeFiles/cs5974_NTS.dir/rand.c.obj.provides.build: CMakeFiles/cs5974_NTS.dir/rand.c.obj


CMakeFiles/cs5974_NTS.dir/CS5974_NTS.c.obj: CMakeFiles/cs5974_NTS.dir/flags.make
CMakeFiles/cs5974_NTS.dir/CS5974_NTS.c.obj: ../CS5974_NTS.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\kdkarki\Documents\workspace\github\CS5974\NoTrustScore\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building C object CMakeFiles/cs5974_NTS.dir/CS5974_NTS.c.obj"
	C:\PROGRA~2\CODEBL~1\MinGW\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles\cs5974_NTS.dir\CS5974_NTS.c.obj   -c C:\Users\kdkarki\Documents\workspace\github\CS5974\NoTrustScore\CS5974_NTS.c

CMakeFiles/cs5974_NTS.dir/CS5974_NTS.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/cs5974_NTS.dir/CS5974_NTS.c.i"
	C:\PROGRA~2\CODEBL~1\MinGW\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E C:\Users\kdkarki\Documents\workspace\github\CS5974\NoTrustScore\CS5974_NTS.c > CMakeFiles\cs5974_NTS.dir\CS5974_NTS.c.i

CMakeFiles/cs5974_NTS.dir/CS5974_NTS.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/cs5974_NTS.dir/CS5974_NTS.c.s"
	C:\PROGRA~2\CODEBL~1\MinGW\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S C:\Users\kdkarki\Documents\workspace\github\CS5974\NoTrustScore\CS5974_NTS.c -o CMakeFiles\cs5974_NTS.dir\CS5974_NTS.c.s

CMakeFiles/cs5974_NTS.dir/CS5974_NTS.c.obj.requires:

.PHONY : CMakeFiles/cs5974_NTS.dir/CS5974_NTS.c.obj.requires

CMakeFiles/cs5974_NTS.dir/CS5974_NTS.c.obj.provides: CMakeFiles/cs5974_NTS.dir/CS5974_NTS.c.obj.requires
	$(MAKE) -f CMakeFiles\cs5974_NTS.dir\build.make CMakeFiles/cs5974_NTS.dir/CS5974_NTS.c.obj.provides.build
.PHONY : CMakeFiles/cs5974_NTS.dir/CS5974_NTS.c.obj.provides

CMakeFiles/cs5974_NTS.dir/CS5974_NTS.c.obj.provides.build: CMakeFiles/cs5974_NTS.dir/CS5974_NTS.c.obj


CMakeFiles/cs5974_NTS.dir/drand48.c.obj: CMakeFiles/cs5974_NTS.dir/flags.make
CMakeFiles/cs5974_NTS.dir/drand48.c.obj: ../drand48.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\kdkarki\Documents\workspace\github\CS5974\NoTrustScore\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building C object CMakeFiles/cs5974_NTS.dir/drand48.c.obj"
	C:\PROGRA~2\CODEBL~1\MinGW\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles\cs5974_NTS.dir\drand48.c.obj   -c C:\Users\kdkarki\Documents\workspace\github\CS5974\NoTrustScore\drand48.c

CMakeFiles/cs5974_NTS.dir/drand48.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/cs5974_NTS.dir/drand48.c.i"
	C:\PROGRA~2\CODEBL~1\MinGW\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E C:\Users\kdkarki\Documents\workspace\github\CS5974\NoTrustScore\drand48.c > CMakeFiles\cs5974_NTS.dir\drand48.c.i

CMakeFiles/cs5974_NTS.dir/drand48.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/cs5974_NTS.dir/drand48.c.s"
	C:\PROGRA~2\CODEBL~1\MinGW\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S C:\Users\kdkarki\Documents\workspace\github\CS5974\NoTrustScore\drand48.c -o CMakeFiles\cs5974_NTS.dir\drand48.c.s

CMakeFiles/cs5974_NTS.dir/drand48.c.obj.requires:

.PHONY : CMakeFiles/cs5974_NTS.dir/drand48.c.obj.requires

CMakeFiles/cs5974_NTS.dir/drand48.c.obj.provides: CMakeFiles/cs5974_NTS.dir/drand48.c.obj.requires
	$(MAKE) -f CMakeFiles\cs5974_NTS.dir\build.make CMakeFiles/cs5974_NTS.dir/drand48.c.obj.provides.build
.PHONY : CMakeFiles/cs5974_NTS.dir/drand48.c.obj.provides

CMakeFiles/cs5974_NTS.dir/drand48.c.obj.provides.build: CMakeFiles/cs5974_NTS.dir/drand48.c.obj


# Object files for target cs5974_NTS
cs5974_NTS_OBJECTS = \
"CMakeFiles/cs5974_NTS.dir/bmeans.c.obj" \
"CMakeFiles/cs5974_NTS.dir/smpl.c.obj" \
"CMakeFiles/cs5974_NTS.dir/stat.c.obj" \
"CMakeFiles/cs5974_NTS.dir/rand.c.obj" \
"CMakeFiles/cs5974_NTS.dir/CS5974_NTS.c.obj" \
"CMakeFiles/cs5974_NTS.dir/drand48.c.obj"

# External object files for target cs5974_NTS
cs5974_NTS_EXTERNAL_OBJECTS =

cs5974_NTS.exe: CMakeFiles/cs5974_NTS.dir/bmeans.c.obj
cs5974_NTS.exe: CMakeFiles/cs5974_NTS.dir/smpl.c.obj
cs5974_NTS.exe: CMakeFiles/cs5974_NTS.dir/stat.c.obj
cs5974_NTS.exe: CMakeFiles/cs5974_NTS.dir/rand.c.obj
cs5974_NTS.exe: CMakeFiles/cs5974_NTS.dir/CS5974_NTS.c.obj
cs5974_NTS.exe: CMakeFiles/cs5974_NTS.dir/drand48.c.obj
cs5974_NTS.exe: CMakeFiles/cs5974_NTS.dir/build.make
cs5974_NTS.exe: CMakeFiles/cs5974_NTS.dir/linklibs.rsp
cs5974_NTS.exe: CMakeFiles/cs5974_NTS.dir/objects1.rsp
cs5974_NTS.exe: CMakeFiles/cs5974_NTS.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=C:\Users\kdkarki\Documents\workspace\github\CS5974\NoTrustScore\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Linking C executable cs5974_NTS.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\cs5974_NTS.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/cs5974_NTS.dir/build: cs5974_NTS.exe

.PHONY : CMakeFiles/cs5974_NTS.dir/build

CMakeFiles/cs5974_NTS.dir/requires: CMakeFiles/cs5974_NTS.dir/bmeans.c.obj.requires
CMakeFiles/cs5974_NTS.dir/requires: CMakeFiles/cs5974_NTS.dir/smpl.c.obj.requires
CMakeFiles/cs5974_NTS.dir/requires: CMakeFiles/cs5974_NTS.dir/stat.c.obj.requires
CMakeFiles/cs5974_NTS.dir/requires: CMakeFiles/cs5974_NTS.dir/rand.c.obj.requires
CMakeFiles/cs5974_NTS.dir/requires: CMakeFiles/cs5974_NTS.dir/CS5974_NTS.c.obj.requires
CMakeFiles/cs5974_NTS.dir/requires: CMakeFiles/cs5974_NTS.dir/drand48.c.obj.requires

.PHONY : CMakeFiles/cs5974_NTS.dir/requires

CMakeFiles/cs5974_NTS.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\cs5974_NTS.dir\cmake_clean.cmake
.PHONY : CMakeFiles/cs5974_NTS.dir/clean

CMakeFiles/cs5974_NTS.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" C:\Users\kdkarki\Documents\workspace\github\CS5974\NoTrustScore C:\Users\kdkarki\Documents\workspace\github\CS5974\NoTrustScore C:\Users\kdkarki\Documents\workspace\github\CS5974\NoTrustScore\cmake-build-debug C:\Users\kdkarki\Documents\workspace\github\CS5974\NoTrustScore\cmake-build-debug C:\Users\kdkarki\Documents\workspace\github\CS5974\NoTrustScore\cmake-build-debug\CMakeFiles\cs5974_NTS.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/cs5974_NTS.dir/depend
