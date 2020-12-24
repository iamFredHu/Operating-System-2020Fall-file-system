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

dir_item *top(int n)
{
    if (path_stack.number == -1 ? 1 : 0)
    {
        return 0;
    }

    return path_stack.number - n >= 0 ? &path_stack.path_t[path_stack.number - n] : &path_stack.path_t[0];
}

//入栈
int push(dir_item *dir_item)
{
    path_stack.number++;
    if (path_stack.number > 19)
    {
        return -1;
    }
    path_stack.path_t[path_stack.number].inode_id = dir_item->inode_id;
    memmove(path_stack.path_t[path_stack.number].name, dir_item->name, 121);
    path_stack.path_t[path_stack.number].type = dir_item->type;
    path_stack.path_t[path_stack.number].valid = dir_item->valid;
    return 0;
}

char *get_file_name(char *path)
{
    int path_len = strlen(path);
    char *start, *end;
    end = start = path + path_len;
    for (int i = path_len - 1; i >= 0; i--)
    {
        if (path[i] != '/')
        {
            start--;
        }
        else
        {
            break;
        }
    }
    int file_name_len = end - start;
    char *file_name = (char *)malloc(file_name_len);
    for (int i = 0; i < file_name_len; i++)
    {
        file_name[i] = start[i];
    }
    return file_name;
}

//拆分path中
int process_cmd(char **ps, char *es, int divide_position, char *divided_str)
{
    char *s;
    //用于暂存s
    char *temp_s;
    //序号
    int index = divide_position;
    //input_command + divide_position 到type以后
    s = *ps + divide_position;

    char *end;
    //如果是特殊符号（包括空格）则++
    while (s < es && strchr(" \t\r\n\v", *s))
    {
        s++;
        index++;
    }
    temp_s = s;
    //找到末尾
    while (s < es && !strchr(" \t\r\n\v", *s))
    {
        s++;
        index++;
    }
    //末尾
    end = s;
    s = temp_s;
    while (s < end)
    {
        *(divided_str++) = *s;
        s++;
    }
    *divided_str = '\0';
    return index;
}

//用于查看path中的"/"
char *watch_path(char **path)
{
    char *start, *end;
    start = *path;
    if (start[0] == '/')
        start++;
    end = start;
    while (*end != '/' && *end != '\0')
    {
        end++;
    }
    int len = end - start;
    char *str = (char *)malloc(len);
    for (int i = 0; i < len; i++)
    {
        str[i] = start[i];
    }
    *path = end;
    return str;
}