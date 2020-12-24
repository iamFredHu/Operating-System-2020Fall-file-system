#ifndef PATH_H
#define PATH_H

#include "init_fs.h"

typedef struct path_stack_t
{
    dir_item stack[20];
    int top;
} path_stack_t;

//路径栈
path_stack_t path_stack;


dir_item *top(int n);
dir_item *pop();
int push(struct dir_item *dir_item);
char *get_file_name(char *path);
char *peek_path(char **path);
int process_path(char **ps, char *es, int divide_position, char *divided_str);
#endif