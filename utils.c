/*
工具函数封装
*/

#include "utils.h"

void push_dir_item(dir_item *push_dir_item_dir_item)
{
    path_t.top_item = path_t.top_item + 1;
    //目录长度过长判断（预设）
    /*
    if(path_t.top_item > 19)
    {
        printf("ERROR:The path is too long.\n");
        return -1;
    }
     */
    path_t.dir_item_stack[path_t.top_item].inode_id = push_dir_item_dir_item->inode_id;
    memmove(path_t.dir_item_stack[path_t.top_item].name, push_dir_item_dir_item->name, 121);
    path_t.dir_item_stack[path_t.top_item].type = push_dir_item_dir_item->type;
    path_t.dir_item_stack[path_t.top_item].valid = push_dir_item_dir_item->valid;
}

dir_item *pop_dir_item()
{
    //为空
    if (path_t.top_item == -1)
    {
        return (dir_item *)0;
    }
    dir_item *pop_dir_item_dir_item = &path_t.dir_item_stack[path_t.top_item];
    path_t.top_item = path_t.top_item - 1;
    return pop_dir_item_dir_item;
}

dir_item *top_dir_item(int number)
{
    if (path_t.top_item == -1)
    {
        return (dir_item *)0;
    }

    if(path_t.top_item - number >= 0)
    {
        return &path_t.dir_item_stack[path_t.top_item-number];
    }
    else
    {
        return &path_t.dir_item_stack[0];
    }
    
}