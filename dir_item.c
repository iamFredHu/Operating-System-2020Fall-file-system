#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "command.h"
#include "disk.h"
#include "inode.h"
#include "init_fs.h"
#include "utils.h"
#include "dir_item.h"
#include "block.h"
#include "path.h"

//dir_item 初始化，即给定相关参数 初始化一个dir_item
dir_item *init_dir_item(uint8_t type, uint32_t inode_id, char *name)
{
    //新建一个临时dir_item
    dir_item *init_dir_item_dir_item = (dir_item *)malloc(128);
    //根据函数参数设置inode_id、type、valid、name信息
    init_dir_item_dir_item->inode_id = inode_id;
    init_dir_item_dir_item->type = type;
    init_dir_item_dir_item->valid = 1;
    memmove(init_dir_item_dir_item->name, name, strlen(name));

    return init_dir_item_dir_item;
}

//根据所给的dir_item和dir_node，将其写入数据块中
int write_dir_item(struct inode *dir_node, struct dir_item *dir_item)
{
    //类型如果是FILE则报错
    if (dir_node->file_type == TYPE_FILE)
        return -1;
    //根据size可以算出inode中的数据块指针id和偏移量
    int block_point_index = dir_node->size / 1024;
    int block_offset = dir_node->size % 1024;

    int block_id = 0;

    //如果数据块指针指向0，说明应该分配一个新的数据块
    if (dir_node->block_point[block_point_index] == 0)
    {
        block_id = search_free_block();
        dir_node->block_point[block_point_index] = block_id;
    }
    //如果数据块指针有指向内容，则修改block_id为该数据块id
    else
    {
        block_id = dir_node->block_point[block_point_index];
    }
    //把内容写入数据块
    write_block(block_id, (char *)dir_item, 128, block_offset);
    //文件大小  //TODO 看看要不要改
    dir_node->size = dir_node->size + 128;
    return 0;
}

//根据文件夹inode读dir_item
dir_item *read_dir_item(inode *dir_node, int block_point_index, int offset_index)
{
    char *read_dir_item_buf = read_block(dir_node->block_point[block_point_index]);
    return (dir_item *)(read_dir_item_buf + offset_index * 128);
}

/**
 * 
 * 在制定inode下寻找dir_item，仅需名字即可
 * 
 * footprint：代表需不需要输出查找过程
 */
//根据dir_name在inode下查找dir_item
dir_item *search_dir_item_in_inode_by_name(inode *dir_inode, char *dir_name, int *blk_index, int *blk_off, int footprint)
{
    int total_dir_item = dir_inode->size / 128;
    *blk_off = 0;
    for (int i = 0; i < 6; i++)
    {
        if (dir_inode->block_point[i] != 0)
        {
            total_dir_item -= 8;
            int dir_item_per_blk = total_dir_item >= 0 ? 8 : total_dir_item + 8;
            int is_last = total_dir_item < 0 ? 1 : 0;
            for (int j = 0; j < dir_item_per_blk; j++)
            {
                dir_item *dir_item = read_dir_item(dir_inode, i, j);
                if (dir_item->valid && footprint)
                {
                    printf("%s %s\n", dir_item->type == TYPE_FILE ? "FILE" : "DIR",dir_item->name);
                }
                if (strcmp(dir_item->name, dir_name) == 0)
                {
                    *blk_index = i;
                    *blk_off = j * 128;
                    return dir_item;
                }
            }
            *blk_off += dir_item_per_blk * 128;
            if (is_last)
            {
                *blk_index = i;
            }
        }
    }
    return (struct dir_item *)0;
}

/**
 * 根据path，找到相应的dir_item
 * ../adadad/adad
 * ./adad/ad
 * addd/add
 * 
 * /dad/ad
 * 
 * path: 路径
 * dir_name：如果没有
 * current_dir_item:用于返回找到的dir_item
 * last_dir_item：用于返回dir_item的上一级dir_item
 * is_follow：是否跟踪路径
 * 
 * return：
 * 没有找到返回-1，否则返回0
 */
int search_dir_item_by_path(char *path, char **dir_name, struct dir_item **current_dir_item, struct dir_item **up_dir_item, int is_follow)
{
    if (strcmp(path, "/") == 0)
    {
        /* / */
        *current_dir_item = root_dir_item;
        *up_dir_item = root_dir_item;
        *dir_name = "/";
        if (is_follow)
        {
            path_stack.top = -1;
            push(root_dir_item);
        }
        return 0;
    }
    else if (path[0] == '/')
    {
        *current_dir_item = root_dir_item;
        if (is_follow)
        {
            path_stack.top = -1;
            push(root_dir_item);
        }
    }
    else
    {
        //get_current_dir_item
        *current_dir_item = top(0);
    }

    while (*dir_name = peek_path(&path))
    {
        if (strlen(*dir_name) == 0)
        {
            break;
        }
        int blk_index = 0;
        int block_off = 0;
        struct inode *current_dir_node = read_inode((*current_dir_item)->inode_id);
        if (current_dir_node->file_type == TYPE_FILE)
        {
            printf("ERROR INFO: %s is not a dir!\n", (*current_dir_item)->name);
            return -1;
        }
        struct dir_item *next_dir_item =
            search_dir_item_in_inode_by_name(current_dir_node, *dir_name, &blk_index, &block_off, 0);
        if (!next_dir_item)
        {
            printf("ERROR INFO: The dir does not exist!\n", *dir_name);
            return -1;
        }
        *up_dir_item = *current_dir_item;
        *current_dir_item = next_dir_item;
        if (next_dir_item->type == TYPE_FILE)
        {
            //printf("find_inode() %s is a file , stop here!\n", *dir_name);
            return 0;
        }
        if (is_follow)
            push(next_dir_item);
    }
    return 0;
}