/*
文件系统的基本操作
*/

#include "filesystem.h"
#include "disk.c"

//文件系统初始化
void init_filesystem()
{

    //调用open_disk函数
    if (open_disk() < 0)
    {
        printf("The disk can't be successfully opened!");
    }

    //read_super_block
    //读magic_num，判断是否为ext2文件系统，若是则打开文件系统，若不是则进行相应的初始化操作
}

//初始化超级块
void init_super_block(sp_block *sb)
{
    sb->magic_num = MAGIC_NUM;
    sb->free_block_count = BLOCK_SIZE - 1 - 32;          //空闲数据块数 超级块占用1 inode数组占用32
    sb->free_inode_count = 32 * BLOCK_SIZE / INODE_SIZE; //空闲inode数
    sb->dir_inode_count = 0;

    //数据块和inode位图初始化
    memset(sb->block_map, 0, sizeof(sb->block_map));
    memset(sb->inode_map, 0, sizeof(sb->inode_map));
}

//获取超级块信息
void info_super_block(sp_block *sb)
{
    if (sb == NULL)
    {
        fprintf(stderr, "Error:The Super Block is null.");
        return;
    }

    int32_t *head = (int32_t *)sb;
    printf(">>>>>>>>>>Super-Block-Information<<<<<<<<<<\n");
    
    if (sb->magic_num == MAGIC_NUM)
    {
        printf("File System:MyEXT2\n");
    }
    else
    {
        printf("File System:UnKnown\n");
    }
    printf("Free block count:%d\n", sb->free_block_count);
    printf("Free inode count:%d\n", sb->free_inode_count);
    printf("Dir inode count:%d\n", sb->dir_inode_count);

    printf(">>>>>>>>>>Super-Block-Information<<<<<<<<<<\n");
}
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
