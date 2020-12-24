#ifndef INIT_FS_H
#define INIT_FS_H

#include <stdint.h>

#define MAGIC_NUM 0xdec0de

#define TYPE_FILE 0
#define TYPE_DIR 1

/* 超级块 */
typedef struct super_block
{
    int32_t magic_num;        // 幻数
    int32_t free_block_count; // 空闲数据块数
    int32_t free_inode_count; // 空闲inode数
    int32_t dir_inode_count;  // 目录inode数
    uint32_t block_map[128];  // 数据块占用位图，共128 * 32 = 4K个数据块
    uint32_t inode_map[32];   // inode占用位图 32 * 32 = 1024个inode节点，与inode共占1k * 32B = 32KB
} sp_block;

/* 文件inode 共 32B，一个数据块可分配1KB，故可分配1KB/32B=32个inode，共需1K/32 = 32个数据块，顺序分配即可*/
typedef struct inode
{
    uint32_t size;                             // 文件大小 4B
    uint16_t file_type;                        // 文件类型（文件/文件夹） 2B
    uint16_t link;                             // 连接数 2B
    uint32_t block_point[6]; // 数据块指针 6 * 4B = 24B
} inode;

/* 目录项 共121 + 4 + 2 + 1 = 128B，一个数据块可分配1KB，故可分配1KB/128B=8个，1个盘块分配4个*/
typedef struct dir_item
{                      // 目录项一个更常见的叫法是 dirent(directory entry)
    uint32_t inode_id; // 当前目录项表示的文件/目录的对应inode   4B
    uint16_t valid;    // 当前目录项是否有效                    2B
    uint8_t type;      // 当前目录项类型（文件/目录）            1B
    char name[121];    // 目录项表示的文件/目录的文件名/目录名    121 * 1B = 121 B
} dir_item;

//超级块
sp_block *sb;
//根目录inode
inode *root_dir_node;
//根目录dir_item
dir_item *root_dir_item;

int init_file_system();

#endif