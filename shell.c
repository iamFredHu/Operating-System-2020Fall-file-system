#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "command.h"
#include "disk.h"
#include "inode.h"
#include "file_system.h"
#include "utils.h"
#include "dir_item.h"
#include "block.h"
#include "path.h"

//全局变量 用于存放指令类型
int command_type = 0;
//全局变量 用于存放路径 用一个二维数组表示
char command_path[20][256];

/* 类型列表
 * 0 - 默认值/指令类型外指令
 * 1 - ls （展示读取文件夹内容）
 * 2 - mkdir （创建文件夹）
 * 3 - touch （创建文件）
 * 4 - cp （复制文件）
 * 5 - shutdown （关闭系统）
 */

void analyse_command(char *input_command, int input_len)
{
    int divide_position = 0;
    int index = 0;
    //记得每次进行分析的时候都要把command_type清零，刚开始写的时候没有清零导致一直报错
    command_type = 0;

    while (divide_position < input_len)
    {
        index++;
        //cmd_type指从输入的cmd中分开得到的type类型指令（ls、mkdir等）
        char *cmd_type = (char *)malloc(256);
        //用divide_path找到type和path分开的位置
        divide_position = process_path(&input_command, input_command + input_len, divide_position, cmd_type);
        //只有第一个空格才属于type
        if (index == 1)
        {
            if (strcmp(cmd_type, "ls") == 0)
            {
                command_type = 1;
            }
            else if (strcmp(cmd_type, "mkdir") == 0)
            {
                command_type = 2;
            }
            else if (strcmp(cmd_type, "touch") == 0)
            {
                command_type = 3;
            }
            else if (strcmp(cmd_type, "cp") == 0)
            {
                command_type = 4;
            }
            else if (strcmp(cmd_type, "shutdown") == 0)
            {
                command_type = 5;
            }
        }
        //因为从index!=1开始，后面再执行的时候得到的就不是type而是path了
        else
        {
            if (strlen(cmd_type) != 0)
                //左移，删除type，剩下的就只是path了
                memmove(command_path[index - 2], cmd_type, strlen(cmd_type));
        }
    }

    //接下来对command_type进行判断和跳转
    if (command_type == 0)
    {
        printf("ERROR: command not defined.\n");
    }
    else if (command_type == 1)
    {
        for (int i = 0; i < 20; i++)
        {
            if (command_path[i][0] != 0)
                ls_cmd(command_path[i]);
            if (command_path[i][0] == 0 && i == 0)
            {
                //如果没有输入path，则进入根目录
                ls_cmd("/");
            }
        }
    }
    else if (command_type == 2)
    {
        for (int i = 0; i < 20; i++)
        {
            if (command_path[i][0] != 0)
                mkdir_cmd(command_path[i]);
        }
    }
    else if (command_type == 3)
    {
        for (int i = 0; i < 20; i++)
        {
            if (command_path[i][0] != 0)
                touch_cmd(command_path[i]);
        }
    }
    else if (command_type == 4)
    {
        cp_cmd(command_path[0], command_path[1]);
    }
    else if (command_type == 5)
    {
        printf("Thank you and see you next time!\n");
        shutdown_cmd();
    }
    else
    {
        printf("System error.\n");
        exit(1);
    }
}

int main(void)
{
    printf("Welcome to my file system!\n");
    //文件系统初始化
    init_file_system();
    //buf，用于存储键盘缓冲区内容
    char command[512];
    //输入内容的长度，注意和前面buf[]的设定大小区分开
    while (1)
    {
        printf("==>");
        memset(command, 0, 512);
        int input_command_len = gets(command, 512);

        //注意如果没有输入内容，应该要求用户再次进行输入
        while (input_command_len == 0)
        {
            printf("Please input your command!\n");
            printf("==>");
            input_command_len = gets(command, 512);
        }
        //对command_path进行初始化
        for (int i = 0; i < 20; i++)
        {
            memset(command_path, 0, 256);
        }
        //对命令进行分析
        analyse_command(command, input_command_len);
        printf("\n");
    }
    return 0;
}
