#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "command.h"
#include "disk.h"
#include "inode.h"
#include "init_fs.h"
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

//根据所给的dir_item和dir_node，将其写入磁盘块中
int write_dir_item(struct inode *dir_node, struct dir_item *dir_item)
{
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
    //把内容写入磁盘块
    write_data_block(block_id, (char *)dir_item, 128, block_offset);
    //写入之后要记得修改文件尺寸
    dir_node->size = dir_node->size + 128;
    return 0;
}

//根据文件夹inode读dir_item
dir_item *read_dir_item(inode *dir_node, int block_point_index, int offset_index)
{
    char *read_dir_item_buf = read_data_block(dir_node->block_point[block_point_index]);
    return (dir_item *)(read_dir_item_buf + offset_index * 128);
}

//根据dir_name在inode下查找dir_item
dir_item *search_dir_item_in_inode_by_name(inode *dir_inode_be_searched, char *dir_name, int *block_id, int *block_offset, int list_flag)
{
    //根据被查找的inode尺寸，算出dir_item的数量
    int dir_item_amount = dir_inode_be_searched->size / 128;
    *block_offset = 0;
    for (int i = 0; i < 6; i++)
    {
        if (dir_inode_be_searched->block_point[i] != 0)
        {
            //每一个数据块的大小是1024B dir_item的大小是128B，因此每查看一个block_point,dir_item_amount就减少8
            dir_item_amount = dir_item_amount - 8;
            //int dir_item_per_block = dir_item_amount >= 0 ? 8 : dir_item_amount + 8;
            int dir_item_per_block;
            int flag_last_item;
            //如果剩余的dir_item >0,说明不是最后一项
            if (dir_item_amount >= 0)
            {
                dir_item_per_block = 8;
                flag_last_item = 0;
            }
            else
            {
                dir_item_per_block = dir_item_amount + 8;
                flag_last_item = 1;
            }

            //int flag_last_item = dir_item_amount < 0 ? 1 : 0;
            for (int j = 0; j < dir_item_per_block; j++)
            {
                //根据inode找dir_item
                dir_item *dir_item = read_dir_item(dir_inode_be_searched, i, j);
                //在执行ls命令时需要列出所有项，就用这个printf实现
                if (dir_item->valid && list_flag)
                {
                    printf("%s %s\n", dir_item->type == TYPE_FILE ? "FILE" : "DIR", dir_item->name);
                }
                //遍历，如果名字相同就返回，同时记录block_id和offset
                if (strcmp(dir_item->name, dir_name) == 0)
                {
                    *block_id = i;
                    *block_offset = j * 128;
                    return dir_item;
                }
            }
            *block_offset += dir_item_per_block * 128;
            //如果已经是最后一项了，就记录为1
            if (flag_last_item)
            {
                *block_id = i;
            }
        }
    }
    return 0;
}

// 根据path，找dir_item path->dir_name->inode->调用上面的函数找dir_item
// current_dir_item:用于返回找到的dir_item
// last_dir_item：用于返回dir_item的上一级dir_item
// 没有找到返回-1，找到返回0

int search_dir_item_by_path(char *path, char **dir_name, struct dir_item **current_dir_item, struct dir_item **up_dir_item)
{
    //如果是根目录，直接已经找到
    if (strcmp(path, "/") == 0)
    {
        *current_dir_item = root_dir_item;
        *up_dir_item = root_dir_item;
        *dir_name = "/";
        return 0;
    }
    //如果path第一个是/，则进入到根目录
    else if (path[0] == '/')
    {
        *current_dir_item = root_dir_item;
    }
    else
    {
        //get_current_dir_item
        *current_dir_item = top(0);
    }

    while ((*dir_name = watch_path(&path)) != NULL)
    {
        if (strlen(*dir_name) == 0)
        {
            break;
        }
        int block_index = 0;
        int block_offset = 0;

        inode *current_dir_node = read_inode((*current_dir_item)->inode_id);

        if (current_dir_node->file_type == TYPE_FILE)
        {
            printf("ERROR INFO: %s is not a dir!\n", (*current_dir_item)->name);
            return -1;
        }

        dir_item *next_dir_item = search_dir_item_in_inode_by_name(current_dir_node, *dir_name, &block_index, &block_offset, 0);

        if (!next_dir_item)
        {
            return -1;
        }

        *up_dir_item = *current_dir_item;
        *current_dir_item = next_dir_item;
        //如果类型为FILE，说明已经到了最后一层，即已经找到
        if (next_dir_item->type == TYPE_FILE)
        {
            return 0;
        }
    }
    return 0;
}