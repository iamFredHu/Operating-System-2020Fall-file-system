/*
对基本的数据结构进行定义
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define MAX_INODE_NUM 1024
#define BLOCK_SIZE 1024

//超级块
typedef struct super_block
{
    int32_t magic_num;        // 幻数
    int32_t free_block_count; // 空闲数据块数
    int32_t free_inode_count; // 空闲inode数
    int32_t dir_inode_count;  // 目录inode数
    uint32_t block_map[128];  // 数据块占用位图
    uint32_t inode_map[32];   // inode占用位图
} sp_block;

//inode数组
typedef struct inode
{
    uint32_t size;           // 文件大小
    uint16_t file_type;      // 文件类型（文件/文件夹）
    uint16_t link;           // 连接数
    uint32_t block_point[6]; // 数据块指针
} inode;

//目录项
typedef struct dir_item
{                      // 目录项一个更常见的叫法是 dirent(directory entry)
    uint32_t inode_id; // 当前目录项表示的文件/目录的对应inode
    uint16_t valid;    // 当前目录项是否有效
    uint8_t type;      // 当前目录项类型（文件/目录）
    char name[121];    // 目录项表示的文件/目录的文件名/目录名
} dir_item;

FILE *fp;
sp_block *spBlock;
inode inode_table[MAX_INODE_NUM];
dir_item block_buffer[BLOCK_SIZE / sizeof(dir_item)];

void screen_information(int num); //屏幕信息显示
void filesystem_init();           //文件系统初始化
void ls();                        //展示读取文件夹内容
void mkdir();                     //创建文件夹
void touch();                     //创建文件
void cp();                        //复制文件
void shutdown();                  //关闭系统
