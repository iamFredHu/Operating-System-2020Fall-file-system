cmake_minimum_required(VERSION 3.0.0)
project(myext2fs VERSION 0.1.0)

include(CTest)
enable_testing()

add_executable(myext2fs shell.c file_system.c inode.c utils.c disk.c command.c dir_item.c block.c)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
