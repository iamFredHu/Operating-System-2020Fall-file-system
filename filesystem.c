/*
文件系统的基本操作
*/

#include "filesystem.h"
#include "disk.c"

//进入文件夹
int ls(char *dir)
{
//TODO 
}   

//创建文件夹     
int mkdir(char *dirName)
{
//TODO 
} 

//创建文件 
int touch(char *fileName)
{
//TODO 
}

//复制文件
int cp()
{
//TODO 
}     

// 关闭系统
void shutdown()
{
    close_disk();
}             
