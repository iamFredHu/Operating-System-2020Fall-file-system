#include "file_system.h"
#include <string.h>
#include <stdlib.h>
/* Copy From XV6 */
int
gets(char *buf, int max)
{
  int i;
  char c;

  for(i=0; i+1 < max; ){
    c = getchar();
    if(c < 0)
      break;
    if(c == '\n' || c == '\r')
      break;
    buf[i++] = c;
  }
  buf[i] = '\0';
  return strlen(buf);
}

void
panic(char * str){
  printf("%s \n", str);
  while (1);
}

char*
peek_path(char **path){
  char *ps, *es;
  ps = *path;
  if(ps[0] == '/')
    ps++;
  es = ps;
  while (*es != '/' && *es != '\0')
  {
    es++;
  }
  int token_len = es - ps;
  char *token = (char *)malloc(token_len);
  for (int i = 0; i < token_len; i++)
  {
    token[i] = ps[i];
  }
  *path = es; 
  return token; 
}

char* 
join(char *s1, char *s2)
{
  char *result = (char *)malloc(strlen(s1)+strlen(s2)+1);
  strcpy(result, s1);
  strcat(result, s2);
  return result;
}

char*
get_file_name(char *path){
  int path_len = strlen(path);
  char *ps, *es;
  es = ps = path + path_len;
  for (int i = path_len - 1; i >= 0; i--)
  {
    if(path[i] != '/'){
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
    /* code */
    name[i] = ps[i];
  }
  return name;
}
