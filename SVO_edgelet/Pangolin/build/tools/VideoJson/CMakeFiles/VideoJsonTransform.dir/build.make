# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

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

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/sam/SVO_edgelet/Pangolin

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/sam/SVO_edgelet/Pangolin/build

# Include any dependencies generated for this target.
include tools/VideoJson/CMakeFiles/VideoJsonTransform.dir/depend.make

# Include the progress variables for this target.
include tools/VideoJson/CMakeFiles/VideoJsonTransform.dir/progress.make

# Include the compile flags for this target's objects.
include tools/VideoJson/CMakeFiles/VideoJsonTransform.dir/flags.make

tools/VideoJson/CMakeFiles/VideoJsonTransform.dir/main-transform.cpp.o: tools/VideoJson/CMakeFiles/VideoJsonTransform.dir/flags.make
tools/VideoJson/CMakeFiles/VideoJsonTransform.dir/main-transform.cpp.o: ../tools/VideoJson/main-transform.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/sam/SVO_edgelet/Pangolin/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object tools/VideoJson/CMakeFiles/VideoJsonTransform.dir/main-transform.cpp.o"
	cd /home/sam/SVO_edgelet/Pangolin/build/tools/VideoJson && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/VideoJsonTransform.dir/main-transform.cpp.o -c /home/sam/SVO_edgelet/Pangolin/tools/VideoJson/main-transform.cpp

tools/VideoJson/CMakeFiles/VideoJsonTransform.dir/main-transform.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/VideoJsonTransform.dir/main-transform.cpp.i"
	cd /home/sam/SVO_edgelet/Pangolin/build/tools/VideoJson && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/sam/SVO_edgelet/Pangolin/tools/VideoJson/main-transform.cpp > CMakeFiles/VideoJsonTransform.dir/main-transform.cpp.i

tools/VideoJson/CMakeFiles/VideoJsonTransform.dir/main-transform.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/VideoJsonTransform.dir/main-transform.cpp.s"
	cd /home/sam/SVO_edgelet/Pangolin/build/tools/VideoJson && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/sam/SVO_edgelet/Pangolin/tools/VideoJson/main-transform.cpp -o CMakeFiles/VideoJsonTransform.dir/main-transform.cpp.s

tools/VideoJson/CMakeFiles/VideoJsonTransform.dir/main-transform.cpp.o.requires:
.PHONY : tools/VideoJson/CMakeFiles/VideoJsonTransform.dir/main-transform.cpp.o.requires

tools/VideoJson/CMakeFiles/VideoJsonTransform.dir/main-transform.cpp.o.provides: tools/VideoJson/CMakeFiles/VideoJsonTransform.dir/main-transform.cpp.o.requires
	$(MAKE) -f tools/VideoJson/CMakeFiles/VideoJsonTransform.dir/build.make tools/VideoJson/CMakeFiles/VideoJsonTransform.dir/main-transform.cpp.o.provides.build
.PHONY : tools/VideoJson/CMakeFiles/VideoJsonTransform.dir/main-transform.cpp.o.provides

tools/VideoJson/CMakeFiles/VideoJsonTransform.dir/main-transform.cpp.o.provides.build: tools/VideoJson/CMakeFiles/VideoJsonTransform.dir/main-transform.cpp.o

# Object files for target VideoJsonTransform
VideoJsonTransform_OBJECTS = \
"CMakeFiles/VideoJsonTransform.dir/main-transform.cpp.o"

# External object files for target VideoJsonTransform
VideoJsonTransform_EXTERNAL_OBJECTS =

tools/VideoJson/VideoJsonTransform: tools/VideoJson/CMakeFiles/VideoJsonTransform.dir/main-transform.cpp.o
tools/VideoJson/VideoJsonTransform: tools/VideoJson/CMakeFiles/VideoJsonTransform.dir/build.make
tools/VideoJson/VideoJsonTransform: src/libpangolin.so
tools/VideoJson/VideoJsonTransform: /usr/lib/x86_64-linux-gnu/libGLU.so
tools/VideoJson/VideoJsonTransform: /usr/lib/x86_64-linux-gnu/libGL.so
tools/VideoJson/VideoJsonTransform: /usr/lib/x86_64-linux-gnu/libSM.so
tools/VideoJson/VideoJsonTransform: /usr/lib/x86_64-linux-gnu/libICE.so
tools/VideoJson/VideoJsonTransform: /usr/lib/x86_64-linux-gnu/libX11.so
tools/VideoJson/VideoJsonTransform: /usr/lib/x86_64-linux-gnu/libXext.so
tools/VideoJson/VideoJsonTransform: /usr/lib/x86_64-linux-gnu/libGLEW.so
tools/VideoJson/VideoJsonTransform: /usr/lib/x86_64-linux-gnu/libSM.so
tools/VideoJson/VideoJsonTransform: /usr/lib/x86_64-linux-gnu/libICE.so
tools/VideoJson/VideoJsonTransform: /usr/lib/x86_64-linux-gnu/libX11.so
tools/VideoJson/VideoJsonTransform: /usr/lib/x86_64-linux-gnu/libXext.so
tools/VideoJson/VideoJsonTransform: /usr/lib/x86_64-linux-gnu/libGLEW.so
tools/VideoJson/VideoJsonTransform: /usr/lib/x86_64-linux-gnu/libpython2.7.so
tools/VideoJson/VideoJsonTransform: /usr/lib/x86_64-linux-gnu/libdc1394.so
tools/VideoJson/VideoJsonTransform: /usr/lib/x86_64-linux-gnu/libavcodec.so
tools/VideoJson/VideoJsonTransform: /usr/lib/x86_64-linux-gnu/libavformat.so
tools/VideoJson/VideoJsonTransform: /usr/lib/x86_64-linux-gnu/libavutil.so
tools/VideoJson/VideoJsonTransform: /usr/lib/x86_64-linux-gnu/libswscale.so
tools/VideoJson/VideoJsonTransform: /usr/lib/libOpenNI.so
tools/VideoJson/VideoJsonTransform: /usr/lib/x86_64-linux-gnu/libpng.so
tools/VideoJson/VideoJsonTransform: /usr/lib/x86_64-linux-gnu/libz.so
tools/VideoJson/VideoJsonTransform: /usr/lib/x86_64-linux-gnu/libjpeg.so
tools/VideoJson/VideoJsonTransform: /usr/lib/x86_64-linux-gnu/libtiff.so
tools/VideoJson/VideoJsonTransform: /usr/lib/x86_64-linux-gnu/libIlmImf.so
tools/VideoJson/VideoJsonTransform: tools/VideoJson/CMakeFiles/VideoJsonTransform.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable VideoJsonTransform"
	cd /home/sam/SVO_edgelet/Pangolin/build/tools/VideoJson && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/VideoJsonTransform.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
tools/VideoJson/CMakeFiles/VideoJsonTransform.dir/build: tools/VideoJson/VideoJsonTransform
.PHONY : tools/VideoJson/CMakeFiles/VideoJsonTransform.dir/build

tools/VideoJson/CMakeFiles/VideoJsonTransform.dir/requires: tools/VideoJson/CMakeFiles/VideoJsonTransform.dir/main-transform.cpp.o.requires
.PHONY : tools/VideoJson/CMakeFiles/VideoJsonTransform.dir/requires

tools/VideoJson/CMakeFiles/VideoJsonTransform.dir/clean:
	cd /home/sam/SVO_edgelet/Pangolin/build/tools/VideoJson && $(CMAKE_COMMAND) -P CMakeFiles/VideoJsonTransform.dir/cmake_clean.cmake
.PHONY : tools/VideoJson/CMakeFiles/VideoJsonTransform.dir/clean

tools/VideoJson/CMakeFiles/VideoJsonTransform.dir/depend:
	cd /home/sam/SVO_edgelet/Pangolin/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/sam/SVO_edgelet/Pangolin /home/sam/SVO_edgelet/Pangolin/tools/VideoJson /home/sam/SVO_edgelet/Pangolin/build /home/sam/SVO_edgelet/Pangolin/build/tools/VideoJson /home/sam/SVO_edgelet/Pangolin/build/tools/VideoJson/CMakeFiles/VideoJsonTransform.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : tools/VideoJson/CMakeFiles/VideoJsonTransform.dir/depend

