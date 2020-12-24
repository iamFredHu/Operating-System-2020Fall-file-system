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
#include "path.h"

dir_item *top(int n)
{
    if (path_stack.top == -1 ? 1 : 0)
    {
        return (struct dir_item *)0;
    }

    return path_stack.top - n >= 0 ? &path_stack.stack[path_stack.top - n] : &path_stack.stack[0];
}

dir_item *pop()
{
    if (path_stack.top == -1 ? 1 : 0)
    {
        return (struct dir_item *)0;
    }
    struct dir_item *item = &path_stack.stack[path_stack.top];
    path_stack.top--;
    return item;
}

int push(struct dir_item *dir_item)
{
    path_stack.top++;
    if (path_stack.top > 19)
    {
        printf("push path() path_stack is too long");
        return -1;
    }
    path_stack.stack[path_stack.top].inode_id = dir_item->inode_id;
    memmove(path_stack.stack[path_stack.top].name, dir_item->name, 121);
    path_stack.stack[path_stack.top].type = dir_item->type;
    path_stack.stack[path_stack.top].valid = dir_item->valid;
    return 0;
}