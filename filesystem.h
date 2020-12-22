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

typedef struct path_transfer
{
    dir_item dir_item_stack[20];
    int top_item;
} path_tf;

sp_block *sb;
inode node[MAX_INODE_NUM];
path_tf path_t;

inode *root_dir_node;
dir_item *root_dir_dir_item;

void info_super_block(sp_block *sb);  //获取超级块信息

char* read_block(int block_number); //读数据块
void write_block(int block_number,char *buf,int size,int offset); //写数据块

inode* read_inode(int inode_number); //读取inode
void write_inode(inode *write_inode_node, int inode_number); //写入inode
inode* init_inode(uint16_t file_type); //初始化inode
uint32_t create_inode(inode *create_inode_node); //创建inode

int find_free_block(); //寻找空闲块
int find_free_inode(); //寻找空闲inode

dir_item* init_dir_item(uint8_t type, uint32_t inode_id, char *name); //创建目录项结构体
void write_dir_item(dir_item* dir,inode* read_dir_item_node); //写目录
dir_item* read_dir_item(inode* read_dir_item_node,int block_point_index,int offset_index); //读目录

void init_filesystem();        //文件系统初始化

int ls(char *dir);             //进入文件夹
inode* mkdir(char *dirName);      //创建文件夹
int touch(char *fileName);     //创建文件
int cp(char *ori, char *dest); //复制文件
void shutdown();               //关闭系统
