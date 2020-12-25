#ifndef PATH_H
#define PATH_H

#include "init_fs.h"

typedef struct path_s
{
    dir_item path_s_dir_item[20];
    int number;
} path_s;

//路径栈
path_s path_stack;

dir_item *path_top(int n);
int path_push(dir_item *dir_item);
char *get_file_name(char *path);
char *watch_path(char **path);
int process_cmd(char **ps, char *es, int divide_position, char *divided_str);

#endif