#ifndef PATH_H
#define PATH_H

#include "file_system.h"
/* 路径栈 */
typedef struct path_stack_t
{
    struct dir_item stack[20];
    int top;
} path_stack_t;

//路径栈
path_stack_t path_stack;

dir_item *top(int n);
dir_item *pop();
int push(struct dir_item *dir_item);

#endif