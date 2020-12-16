/*
对基本的数据结构进行定义
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define MAX_INODE_NUM 1024
#define MAGIC_NUM 14598366
#define MAX_NUM 1 << 31

#define TYPE_FILE 0
#define TYPE_DIR 1

#define INODE_START 2

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

sp_block sb;
inode node[MAX_INODE_NUM];

void init_super_block(sp_block *sb);  //初始化超级块
void read_super_block(sp_block *sb);  //读取超级块
void write_super_block(sp_block *sb); //写入超级块
void info_super_block(sp_block *sb);  //获取超级块信息

void read_super_block_disk(char *buf);  //在读取超级块时，从disk中读buf 对函数进行封装
void write_super_block_disk(char *buf); //在写入超级块时，将buf写入disk中 对函数进行封装

void read_inode(inode *node, int block_number);  //读取inode
void write_inode(inode *node, int block_number); //写入inode

int find_free_block(); //寻找空闲块
int find_free_inode(); //寻找空闲inode

void init_filesystem();        //文件系统初始化

int ls(char *dir);             //进入文件夹
int mkdir(char *dirName);      //创建文件夹
int touch(char *fileName);     //创建文件
int cp(char *ori, char *dest); //复制文件
void shutdown();               //关闭系统
