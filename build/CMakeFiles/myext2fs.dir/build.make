# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.13

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

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/180110505/Operating-System-2020Fall-file-system

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/180110505/Operating-System-2020Fall-file-system/build

# Include any dependencies generated for this target.
include CMakeFiles/myext2fs.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/myext2fs.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/myext2fs.dir/flags.make

CMakeFiles/myext2fs.dir/shell.c.o: CMakeFiles/myext2fs.dir/flags.make
CMakeFiles/myext2fs.dir/shell.c.o: ../shell.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/180110505/Operating-System-2020Fall-file-system/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/myext2fs.dir/shell.c.o"
	/bin/gcc-8 $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/myext2fs.dir/shell.c.o   -c /home/180110505/Operating-System-2020Fall-file-system/shell.c

CMakeFiles/myext2fs.dir/shell.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/myext2fs.dir/shell.c.i"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_C_CREATE_PREPROCESSED_SOURCE

CMakeFiles/myext2fs.dir/shell.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/myext2fs.dir/shell.c.s"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_C_CREATE_ASSEMBLY_SOURCE

CMakeFiles/myext2fs.dir/file_system.c.o: CMakeFiles/myext2fs.dir/flags.make
CMakeFiles/myext2fs.dir/file_system.c.o: ../file_system.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/180110505/Operating-System-2020Fall-file-system/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/myext2fs.dir/file_system.c.o"
	/bin/gcc-8 $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/myext2fs.dir/file_system.c.o   -c /home/180110505/Operating-System-2020Fall-file-system/file_system.c

CMakeFiles/myext2fs.dir/file_system.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/myext2fs.dir/file_system.c.i"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_C_CREATE_PREPROCESSED_SOURCE

CMakeFiles/myext2fs.dir/file_system.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/myext2fs.dir/file_system.c.s"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_C_CREATE_ASSEMBLY_SOURCE

CMakeFiles/myext2fs.dir/inode.c.o: CMakeFiles/myext2fs.dir/flags.make
CMakeFiles/myext2fs.dir/inode.c.o: ../inode.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/180110505/Operating-System-2020Fall-file-system/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object CMakeFiles/myext2fs.dir/inode.c.o"
	/bin/gcc-8 $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/myext2fs.dir/inode.c.o   -c /home/180110505/Operating-System-2020Fall-file-system/inode.c

CMakeFiles/myext2fs.dir/inode.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/myext2fs.dir/inode.c.i"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_C_CREATE_PREPROCESSED_SOURCE

CMakeFiles/myext2fs.dir/inode.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/myext2fs.dir/inode.c.s"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_C_CREATE_ASSEMBLY_SOURCE

CMakeFiles/myext2fs.dir/utils.c.o: CMakeFiles/myext2fs.dir/flags.make
CMakeFiles/myext2fs.dir/utils.c.o: ../utils.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/180110505/Operating-System-2020Fall-file-system/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object CMakeFiles/myext2fs.dir/utils.c.o"
	/bin/gcc-8 $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/myext2fs.dir/utils.c.o   -c /home/180110505/Operating-System-2020Fall-file-system/utils.c

CMakeFiles/myext2fs.dir/utils.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/myext2fs.dir/utils.c.i"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_C_CREATE_PREPROCESSED_SOURCE

CMakeFiles/myext2fs.dir/utils.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/myext2fs.dir/utils.c.s"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_C_CREATE_ASSEMBLY_SOURCE

CMakeFiles/myext2fs.dir/disk.c.o: CMakeFiles/myext2fs.dir/flags.make
CMakeFiles/myext2fs.dir/disk.c.o: ../disk.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/180110505/Operating-System-2020Fall-file-system/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building C object CMakeFiles/myext2fs.dir/disk.c.o"
	/bin/gcc-8 $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/myext2fs.dir/disk.c.o   -c /home/180110505/Operating-System-2020Fall-file-system/disk.c

CMakeFiles/myext2fs.dir/disk.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/myext2fs.dir/disk.c.i"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_C_CREATE_PREPROCESSED_SOURCE

CMakeFiles/myext2fs.dir/disk.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/myext2fs.dir/disk.c.s"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_C_CREATE_ASSEMBLY_SOURCE

CMakeFiles/myext2fs.dir/command.c.o: CMakeFiles/myext2fs.dir/flags.make
CMakeFiles/myext2fs.dir/command.c.o: ../command.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/180110505/Operating-System-2020Fall-file-system/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building C object CMakeFiles/myext2fs.dir/command.c.o"
	/bin/gcc-8 $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/myext2fs.dir/command.c.o   -c /home/180110505/Operating-System-2020Fall-file-system/command.c

CMakeFiles/myext2fs.dir/command.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/myext2fs.dir/command.c.i"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_C_CREATE_PREPROCESSED_SOURCE

CMakeFiles/myext2fs.dir/command.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/myext2fs.dir/command.c.s"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_C_CREATE_ASSEMBLY_SOURCE

CMakeFiles/myext2fs.dir/dir_item.c.o: CMakeFiles/myext2fs.dir/flags.make
CMakeFiles/myext2fs.dir/dir_item.c.o: ../dir_item.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/180110505/Operating-System-2020Fall-file-system/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building C object CMakeFiles/myext2fs.dir/dir_item.c.o"
	/bin/gcc-8 $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/myext2fs.dir/dir_item.c.o   -c /home/180110505/Operating-System-2020Fall-file-system/dir_item.c

CMakeFiles/myext2fs.dir/dir_item.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/myext2fs.dir/dir_item.c.i"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_C_CREATE_PREPROCESSED_SOURCE

CMakeFiles/myext2fs.dir/dir_item.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/myext2fs.dir/dir_item.c.s"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_C_CREATE_ASSEMBLY_SOURCE

CMakeFiles/myext2fs.dir/block.c.o: CMakeFiles/myext2fs.dir/flags.make
CMakeFiles/myext2fs.dir/block.c.o: ../block.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/180110505/Operating-System-2020Fall-file-system/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building C object CMakeFiles/myext2fs.dir/block.c.o"
	/bin/gcc-8 $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/myext2fs.dir/block.c.o   -c /home/180110505/Operating-System-2020Fall-file-system/block.c

CMakeFiles/myext2fs.dir/block.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/myext2fs.dir/block.c.i"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_C_CREATE_PREPROCESSED_SOURCE

CMakeFiles/myext2fs.dir/block.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/myext2fs.dir/block.c.s"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_C_CREATE_ASSEMBLY_SOURCE

# Object files for target myext2fs
myext2fs_OBJECTS = \
"CMakeFiles/myext2fs.dir/shell.c.o" \
"CMakeFiles/myext2fs.dir/file_system.c.o" \
"CMakeFiles/myext2fs.dir/inode.c.o" \
"CMakeFiles/myext2fs.dir/utils.c.o" \
"CMakeFiles/myext2fs.dir/disk.c.o" \
"CMakeFiles/myext2fs.dir/command.c.o" \
"CMakeFiles/myext2fs.dir/dir_item.c.o" \
"CMakeFiles/myext2fs.dir/block.c.o"

# External object files for target myext2fs
myext2fs_EXTERNAL_OBJECTS =

myext2fs: CMakeFiles/myext2fs.dir/shell.c.o
myext2fs: CMakeFiles/myext2fs.dir/file_system.c.o
myext2fs: CMakeFiles/myext2fs.dir/inode.c.o
myext2fs: CMakeFiles/myext2fs.dir/utils.c.o
myext2fs: CMakeFiles/myext2fs.dir/disk.c.o
myext2fs: CMakeFiles/myext2fs.dir/command.c.o
myext2fs: CMakeFiles/myext2fs.dir/dir_item.c.o
myext2fs: CMakeFiles/myext2fs.dir/block.c.o
myext2fs: CMakeFiles/myext2fs.dir/build.make
myext2fs: CMakeFiles/myext2fs.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/180110505/Operating-System-2020Fall-file-system/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Linking C executable myext2fs"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/myext2fs.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/myext2fs.dir/build: myext2fs

.PHONY : CMakeFiles/myext2fs.dir/build

CMakeFiles/myext2fs.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/myext2fs.dir/cmake_clean.cmake
.PHONY : CMakeFiles/myext2fs.dir/clean

CMakeFiles/myext2fs.dir/depend:
	cd /home/180110505/Operating-System-2020Fall-file-system/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/180110505/Operating-System-2020Fall-file-system /home/180110505/Operating-System-2020Fall-file-system /home/180110505/Operating-System-2020Fall-file-system/build /home/180110505/Operating-System-2020Fall-file-system/build /home/180110505/Operating-System-2020Fall-file-system/build/CMakeFiles/myext2fs.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/myext2fs.dir/depend

