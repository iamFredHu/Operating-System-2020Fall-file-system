/*
文件系统Shell
*/

#include "filesystem.c"
#include "shell.h"

//从键盘输入流中获取命令
int get_cmd(char *buf, int buf_size)
{
    printf("==> ");              //输入提示符
    memset(buf, 0, buf_size);    //buf数组进行置0初始化
    fgets(buf, buf_size, stdin); // 从指定的流（buf）中读取数据
    if (buf[0] == 0)
        return -1; //没有buf读入时返回-1，shell结束程序
    return 0;
}

//处理获取到的命令
int process_cmd(char *buf)
{
    char path[50];
    char command[50];
    int i = 0;

    /*
    对获取到的buf进行分类，可分为path（路径）和command（命令）2类
    对于一个指令，例如：cmd a/b/c.txt
    cmd属于command，而a/b/c.txt属于path，只要检测到有空格，就可以认为空格之前的是command（命令）
    对于后面没有path（路径）的命令来说，检测到换行符号（\n）可认为指令结束
    */
    for (; i <= strlen(buf); i++)
    {
        if (buf[i] == ' ' || buf[i] == '\n')
        {
            strncpy(command, buf, i);
            command[i] = '\0';
            break;
        }
    }
    if (strcmp("ls", command) == 0)
    {
        strcpy(path, buf + i + 1);
        path[strlen(path) - 1] = '\0';
        //ls(path);
        printf("ls shelltest %s\n", path);
    }
    else if (strcmp("mkdir", command) == 0)
    {
        strcpy(path, buf + i + 1);
        path[strlen(path) - 1] = '\0';
        //mkdir(path);
        printf("mkdir shelltest %s\n", path);
    }
    else if (strcmp("touch", command) == 0)
    {
        strcpy(path, buf + i + 1);
        path[strlen(path) - 1] = '\0';
        //touch(path);
        printf("touch shelltest %s\n", path);
    }
    else if (strcmp("cp", command) == 0)
    {
        strcpy(path, buf + i + 1);
        path[strlen(path) - 1] = '\0';
        //cp(path);
        printf("cp shelltest %s\n", path);
    }
    else if (strcmp("shutdown", command) == 0)
    {
        shutdown();
        return 0;
    }
    else
    {
        printf("Error Command.\n");
        return 1;
    }
}

int main(void)
{
    static char buf[100]; //键盘缓冲区读入

    init_filesystem(); //文件系统初始化
    find_free_block();
    find_free_inode();
    printf("Welcome to my file system!\n");
    while (get_cmd(buf, sizeof(buf)) >= 0) //如果能读到buf，则一直循环下去
    {
        if (process_cmd(buf) == 0) //如果process_cmd函数读不到buf，则退出程序
        {
            printf("Thank you,and see you next time!\n");
            return 0;
        }
    }
    return 0;
}
