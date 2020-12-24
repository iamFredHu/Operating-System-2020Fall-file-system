#ifndef UTILS_H
#define UTILS_H

int gets(char *buf, int max);
void panic(char *str);
char *peek_path(char **path);
char *join(char *s1, char *s2);
char *get_file_name(char *path);

#endif