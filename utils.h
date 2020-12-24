#ifndef UTILS_H
#define UTILS_H

int gets(char *buf, int max);
char *peek_path(char **path);
char *get_file_name(char *path);
int process_path(char **ps, char *es, int divide_position, char *divided_str);

#endif