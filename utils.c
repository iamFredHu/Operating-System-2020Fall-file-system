#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "command.h"
#include "disk.h"
#include "inode.h"
#include "init_fs.h"
#include "utils.h"
#include "dir_item.h"
#include "block.h"
#include "path.h"
/* Copy From XV6 */
int gets(char *buf, int max)
{
  int i;
  char c;

  for (i = 0; i + 1 < max;)
  {
    c = getchar();
    if (c < 0)
      break;
    if (c == '\n' || c == '\r')
      break;
    buf[i++] = c;
  }
  buf[i] = '\0';
  return strlen(buf);
}

char *peek_path(char **path)
{
  char *ps, *es;
  ps = *path;
  if (ps[0] == '/')
    ps++;
  es = ps;
  while (*es != '/' && *es != '\0')
  {
    es++;
  }
  int token_len = es - ps;
  char *str = (char *)malloc(token_len);
  for (int i = 0; i < token_len; i++)
  {
    str[i] = ps[i];
  }
  *path = es;
  return str;
}

char *get_file_name(char *path)
{
  int path_len = strlen(path);
  char *ps, *es;
  es = ps = path + path_len;
  for (int i = path_len - 1; i >= 0; i--)
  {
    if (path[i] != '/')
    {
      ps--;
    }
    else
    {
      break;
    }
  }
  int name_len = es - ps;

  char *name = (char *)malloc(name_len);
  for (int i = 0; i < name_len; i++)
  {
    name[i] = ps[i];
  }
  return name;
}

int process_path(char **ps, char *es, int divide_position, char *divided_str)
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