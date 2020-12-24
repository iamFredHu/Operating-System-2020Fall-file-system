#ifndef INIT_FS_H
#define INIT_FS_H

#include <stdint.h>

#define MAGIC_NUM 0xdec0de

#define TYPE_FILE 0
#define TYPE_DIR 1

//超级块的数据结构定义
typedef struct super_block
{
    int32_t magic_num;        // 幻数
    int32_t free_block_count; // 空闲数据块数
    int32_t free_inode_count; // 空闲inode数
    int32_t dir_inode_count;  // 目录inode数
    uint32_t block_map[128];  // 数据块占用位图
    uint32_t inode_map[32];   // inode占用位图
} sp_block;

//inode结构体定义
typedef struct inode
{
    uint32_t size;           // 文件大小
    uint16_t file_type;      // 文件类型（文件/文件夹）
    uint16_t link;           // 连接数
    uint32_t block_point[6]; // 数据块指针
} inode;


typedef struct dir_item
{                      // 目录项一个更常见的叫法是 dirent(directory entry)
    uint32_t inode_id; // 当前目录项表示的文件/目录的对应inode
    uint16_t valid;    // 当前目录项是否有效
    uint8_t type;      // 当前目录项类型（文件/目录）
    char name[121];    // 目录项表示的文件/目录的文件名/目录名
} dir_item;

//超级块
sp_block *sb;
//根目录inode
inode *root_dir_node;
//根目录dir_item
dir_item *root_dir_item;

int init_file_system();

#endif