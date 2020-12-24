#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "command.h"
#include "disk.h"
#include "inode.h"
#include "file_system.h"
#include "utils.h"
#include "dir_item.h"
#include "block.h"
#include "path.h"

void shutdown_cmd()
{
    close_disk();
    exit(0);
}

//进入目录
//输入 *path 路径
void ls_cmd(char *path)
{
    //目录名
    char *dir_name;
    //当前目录的dir_item
    struct dir_item *cur_dir_item;
    //当前目录的inode
    struct inode *cur_inode;
    //上级目录的dir_item
    struct dir_item *up_dir_item;
    //根据path找到对应的dir_item
    search_dir_item_by_path(path, &dir_name, &cur_dir_item, &up_dir_item, 0);
    //根据当前目录的dir_item，有对应的inode_id，根据inode_id可以找到对应的inode
    cur_inode = read_inode(cur_dir_item->inode_id);
    int block_index = 0;
    int block_offset = 0;
    //根据inode查找相应的dir_item
    search_dir_item_in_inode_by_name(cur_inode, "/", &block_index, &block_offset, 1);
}

//创建文件
//输入 *path 路径
inode *touch_cmd(char *path)
{
    //当前目录的dir_item
    dir_item *cur_dir_item;
    //上级目录的dir_item
    dir_item *up_dir_item;
    //当前目录的inode
    inode *cur_inode;
    //下级目录的inode
    inode *down_inode;
    //目录名
    char *dir_name;
    //从路径中判断出文件名
    char *file_name = get_file_name(path);

    //根据目录找到dir_item,如果没有找到则需要进行创建，如果找到了直接结束，并给出用户反馈
    if (search_dir_item_by_path(path, &dir_name, &cur_dir_item, &up_dir_item, 0) < 0)
    {
        if (memcmp(dir_name, file_name, strlen(file_name)) != 0)
        {
            printf("ERROR INFO: dir_name %s error!\n", dir_name);
            return 0;
        }
        /* 创建不存在的目录 */
        inode *down_inode = init_inode(TYPE_FILE);
        uint32_t inode_index = write_inode(down_inode);
        dir_item *dir_item = init_dir_item(TYPE_FILE, inode_index, dir_name);
        /* 刷新当前节点 */
        cur_inode = read_inode(cur_dir_item->inode_id);
        write_dir_item(cur_inode, dir_item);
        sync_inode(cur_dir_item->inode_id, cur_inode);

        return down_inode;
    }
    else
    {
        printf("ERROR INFO:The file has existed.!\n");
    }
}

inode *mkdir_cmd(char *path)
{
    sb->dir_inode_count++;
    /* 根目录 */
    if (strcmp(path, "/") == 0)
    {
        struct inode *dir_node = init_inode(TYPE_DIR);
        if (root_dir_node)
        {
            printf("mkdir_cmd() can not create another root directory! \n");
            return (struct inode *)0;
        }
        write_inode(dir_node);
        disk_write_block(0, (char *)sb);
        disk_write_block(1, (char *)sb + DEVICE_BLOCK_SIZE);
        return dir_node;
    }
    /* 非根目录 */
    else
    {
        /**
         * TODO
         * 
         * 基本逻辑：
         * 找到上一级dir_item，顺便找到它的inode，为其创建dir_item，
         * 
         * 记得每次更新 inode 和 
         */
        /* 测试，创建一个DIR */
        struct dir_item *current_dir_item;
        struct dir_item *up_dir_item;
        struct inode *current_inode;
        struct inode *down_inode;
        char *dir_name;
        printf("path %s \n", path);
        if (search_dir_item_by_path(path, &dir_name, &current_dir_item, &up_dir_item, 0) < 0)
        {
            printf("mkdir_cmd() not find path: %s\n", path);
            printf("mkdir_cmd() now we'll create one for you !\n");

            /**
             *  
             * 创建不存在的目录
             * 
             * dir_item -> block -> dir .
             *                   -> dir ..
             * */
            struct inode *down_inode = init_inode(TYPE_DIR);
            uint32_t inode_index = write_inode(down_inode);
            struct dir_item *dir_item = init_dir_item(TYPE_DIR, inode_index, dir_name);

            sync_inode(dir_item->inode_id, down_inode);
            /* 刷新当前节点 */
            /**
             * 刷新current_inode即可
             * current_dir_item -> current_inode -> dir_item -> down_inode -> dir .
             *                                                             -> dir ..
             */
            current_inode = read_inode(current_dir_item->inode_id);
            write_dir_item(current_inode, dir_item);
            sync_inode(current_dir_item->inode_id, current_inode);

            down_inode = mkdir_cmd(path);
            disk_write_block(0, (char *)sb);
            disk_write_block(1, (char *)sb + DEVICE_BLOCK_SIZE);
            return down_inode;
        }
    }
}

int cp_cmd(char *from_path, char *to_path)
{

    struct dir_item *from_current_dir_item;
    struct dir_item *from_last_dir_item;
    struct inode *from_current_inode;

    struct dir_item *to_current_dir_item;
    struct dir_item *to_last_dir_item;
    struct inode *to_current_inode;

    char *from_dir_name;
    char *to_dir_name;

    printf("from_path %s , to_path %s\n", from_path, to_path);

    if (search_dir_item_by_path(from_path, &from_dir_name, &from_current_dir_item, &from_last_dir_item, 0) < 0)
    {
        printf("cp_cmd() source file not exist!\n");
        return -1;
    }
    if (search_dir_item_by_path(to_path, &to_dir_name, &to_current_dir_item, &to_last_dir_item, 0) == 0)
    {
        printf("cp_cmd() dest file exists!\n");
        return -1;
    }
    from_current_inode = read_inode(from_current_dir_item->inode_id);
    to_current_inode = touch_cmd(to_path);
    if (!to_current_inode)
    {
        printf("cp_cmd() dest directory not exists!\n");
        return -1;
    }
    search_dir_item_by_path(to_path, &to_dir_name, &to_current_dir_item, &to_last_dir_item, 0);
    for (int i = 0; i < 6; i++)
    {
        uint32_t origin_block_num = from_current_inode->block_point[i];
        if (origin_block_num != 0)
        {
            uint32_t block_id = search_free_block();
            to_current_inode->block_point[i] = block_id;
            char *buf = read_block(origin_block_num);
            write_block(block_id, buf, 1024, 0);
        }
    }
    sync_inode(to_current_dir_item->inode_id, to_current_inode);
    return 0;
}
