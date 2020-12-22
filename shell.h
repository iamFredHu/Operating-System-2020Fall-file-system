#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


int get_cmd(char *buf, int buf_size); //从键盘输入流中获取命令

int process_cmd(char *buf); //处理获取到的命令