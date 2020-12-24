#ifndef PATH_H
#define PATH_H

#include "init_fs.h"

typedef struct path_stack_t
{
    dir_item path_t[20];
    int number;
} path_stack_t;

//路径栈
path_stack_t path_stack;

dir_item *top(int n);
int push(dir_item *dir_item);
char *get_file_name(char *path);
char *watch_path(char **path);
int process_cmd(char **ps, char *es, int divide_position, char *divided_str);

#endif