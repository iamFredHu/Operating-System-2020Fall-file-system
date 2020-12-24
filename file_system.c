#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "command.h"
#include "disk.h"
#include "inode.h"
#include "file_system.h"
#include "utils.h"
#include "dir_item.h"
#include "block.h"
#include "path.h"

//文件系统初始化
int init_file_system()
{
    if (open_disk() < 0)
    {
        printf("The disk can't be opened.\n");
    }
    else
    {
        printf("The disk has been successfully opened!\n");
    }

    char *init_filesystem_buf;
    //读block_number为0的块
    init_filesystem_buf = read_block(0);
    sb = (sp_block *)init_filesystem_buf;
    //读MagicNumber，如果符合MyEXT2的MagicNumber，就认为是已经有内容，反之则进行初始化
    if (sb->magic_num != MAGIC_NUM) //不符合的话，进行初始化
    {
        //设置一个变量format_flag,用于循环询问用户是否进行格式化
        int format_flag = 1;
        while (format_flag)
        {
            printf("The file system is unknown,format the disk and rebuild a new ext2 file system?(y/n)");
            char input_choice = getchar();
            getchar();
            if (input_choice == 'y')
            {
                //格式化相关操作

                //根据指导书中的示例，初始空闲数据块数量为4096，初始空闲inode数量为1024
                sb->magic_num = MAGIC_NUM;
                sb->dir_inode_count = 0;
                sb->free_block_count = 4096;
                sb->free_inode_count = 1024;

                //对block_map和inode_map置0初始化
                memset(sb->block_map, 0, 512); //Why 512? block_map 128*4(uint32占4个字节)
                memset(sb->inode_map, 0, 128); //Why 128? inode_map 32*4(uint32占4个字节)

                //标记占用情况 superblock占用block0，inode占用block 1-32
                for (int i = 0; i < 33; i++)
                {
                    alloc_block(i);
                }
                //设置根目录项
                root_dir_node = mkdir_cmd("/");
                printf("The file system has been formatted,have a good time!\n");
                format_flag = 0;
            }
            else if (input_choice == 'n')
            {
                printf("Thank you,and see you next time!\n");
                shutdown_cmd();
            }
            else
            {
                printf("Error choice,please input your choice again!\n");
            }
        }
    }
    //初始化完毕后，或是已有信息，就可以进入文件系统了

    //从inode number为0的地方读取根目录inode，所以上面格式化的时候就要设置一个根目录的inode
    root_dir_node = read_inode(0);
    //根目录dir_item初始化
    root_dir_item = init_dir_item(TYPE_DIR, 0, "/");
    //目录栈初始化
    path_stack.top = -1;
    push(root_dir_item);
    //初始时应该进入根目录
    ls_cmd("/");
    return 0;
}