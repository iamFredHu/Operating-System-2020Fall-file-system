#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "command.h"
#include "disk.h"
#include "inode.h"
#include "file_system.h"
#include "utils.h"
#include "dir_item.h"
#include "block.h"

//inode 初始化，即给定相关参数 初始化一个inode
inode *init_inode(uint16_t file_type)
{
    //新建一个临时inode
    inode *init_inode_inode = (inode *)malloc(32);
    //根据函数参数设置file_type、size、link、block_point信息
    init_inode_inode->file_type = file_type;
    init_inode_inode->size = 0;
    init_inode_inode->link = 1;
    for (int i = 0; i < 6; i++)
    {
        init_inode_inode->block_point[i] = 0;
    }
    return init_inode_inode;
}

//找空闲的inode
/* 如何找空闲的inode呢？
 * 根据指导书，我们有inode_map的每一比特记录了数据块的占用情况，在指导书的示例中，可以表示32*32=1024个inode的占用情况
 * 所以我们将inode_map按位与，如果结果为0，则说明对应的inode为空闲的
 * 返回inode_map位图第多少个比特为空闲
 */
uint32_t get_free_inode()
{
    for (uint32_t i = 0; i < 32; i++)
    {
        for (uint32_t j = 0; j < 32; j++)
        {
            if (((sb->inode_map[i] >> j) & 1) == 0)
            {
                printf("find one inode: %d\n", i * 32 + j);
                return i * 32 + j;
            }
        }
    }
}

//分配inode
void alloc_inode(uint32_t inode_number)
{
    //inode_number是指inode编号 如果要根据inode编号分配inode，要修改inode_map
    int inode_map_index = inode_number / 32;
    int32_t bit_index = inode_number % 32;
    //inode_map 32*32 inode_map_index是inode_map[]的index bit_index是uint32_t每一bit的index
    sb->inode_map[inode_map_index] |= 1 << bit_index;
    //同时要减少空闲inode数
    sb->free_inode_count--;
    //写入磁盘
    disk_write_block(0, (char *)sb);
    disk_write_block(1, (char *)sb + DEVICE_BLOCK_SIZE);
}

//将inode写入磁盘
uint32_t write_inode(struct inode *inode)
{
    //首先找到空闲的inode
    uint32_t free_inode_index = get_free_inode();
    //数据块id //TODO 为什么要+1
    int block_id = free_inode_index / 32 + 1;
    //位图比特id
    int bit_index = free_inode_index % 32;
    //分配inode
    alloc_inode(free_inode_index);
    //写入磁盘
    write_block(block_id, (char *)inode, 32, bit_index * 32);
    return free_inode_index;
}

/**
 * 
 * inode_num：要更新的inode编号
 * update_inode：要更新的inode
 */
int sync_inode(uint32_t inode_id, struct inode *update_inode)
{
    int block_id = inode_id / 32 + 1; //第几个数据块
    int bit_index = inode_id % 32;    //数据块内偏移
    write_block(block_id, (char *)update_inode, 32, bit_index * 32);
}

//根据inode_id,返回相应的inode地址
inode *read_inode(uint32_t inode_id)
{
    //数据块id 需要注意第一个数据块id被超级块占用，所以需要—+1
    int block_id = (inode_id / 32) + 1;
    //数据块内偏移量，一个inode占用大小为32B，一个数据块大小为512B
    int bit_index = inode_id % 32;

    //根据数据块id读数据块，然后再把读到的buf强转为inode
    char *read_inode_buf;
    inode *read_inode_node;
    read_inode_buf = read_block(block_id);
    //注意加上偏移量
    read_inode_node = (inode *)(read_inode_buf + bit_index * 32);

    return read_inode_node;
}