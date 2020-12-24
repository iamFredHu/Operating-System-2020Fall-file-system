cmake_minimum_required(VERSION 3.0.0)
project(MyExt2FS VERSION 0.1.0)

include(CTest)
enable_testing()

add_executable(MyExt2FS shell.c init_fs.c inode.c disk.c command.c dir_item.c block.c path.c)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
