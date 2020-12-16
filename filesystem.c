/*
文件系统的基本操作
*/

#include "filesystem.h"
#include "disk.c"

//文件系统初始化
void init_filesystem()
{

    //调用open_disk函数 打开磁盘文件
    if (open_disk() < 0)
    {
        printf("The disk can't be successfully opened!\n");
    }
    else
    {
        printf("The disk has been successfully opened!\n"); //调试信息，等删除
    }

    //read_super_block
    //读magic_num，判断是否为ext2文件系统，若是则打开文件系统，若不是则进行相应的初始化操作
    char *buf;
    buf = read_block(0);
    sb = (sp_block *)buf;
    if (sb->magic_num == MAGIC_NUM)
    {
        //是ext2文件系统，所以可以打开文件系统，读取里面的内容
        info_super_block(sb);
        printf("The file system has been opened successfully...\n");
    }
    else
    {
        int format_flag = 1;
        while (format_flag)
        {
            printf("The file system is unknown,format the disk and rebuild a new ext2 file system?(y/n)");
            char input_choice = getchar();
            getchar();
            if (input_choice == 'y')
            {
                //进行文件系统初始化

                //超级块初始化
                init_super_block(sb);
                info_super_block(sb);

                printf("Formating finished,good luck and have fun!\n");
                format_flag = 0;
            }
            else if (input_choice == 'n')
            {
                printf("Thank you,and see you next time!\n");
                shutdown();
            }
            else
            {
                printf("Error choice,please input your choice again!\n");
            }
        }
    }
}

//初始化超级块
void init_super_block(sp_block *sb)
{
    sb->magic_num = MAGIC_NUM;
    sb->free_block_count = 4096 - 1 - 32; //空闲数据块数
    sb->free_inode_count = 1024;          //空闲inode数 在这里我们的节点数是32*32
    sb->dir_inode_count = 0;

    /*
    * 有关数据块大小的计算
    * 示例中一共可以表示128*32=4096个数据块 其中超级块和inode也需要占用数据块
    * 每个数据块的大小为1KiB
    * 超级块占用1个数据块 inode占用 32 * 1024(1024个inode，每个inode占用32B空间) / 1024 KiB  
    */

    //数据块和inode位图初始化
    memset(sb->block_map, 0, sizeof(sb->block_map));
    memset(sb->inode_map, 0, sizeof(sb->inode_map));

    //表明数据块已占用， 0-super bock 1-32 inode
    for (int i = 0; i < 33; i++)
    {
        //对block_map按位操作
        int index = i / 32;
        int32_t offset = i % 32;
        uint32_t block_map_process = 1 << offset;
        sb->block_map[index] |= block_map_process;
        sb->free_block_count = sb->free_block_count - 1;
        disk_write_block(0, (char *)sb);
        disk_write_block(1, (char *)sb + DEVICE_BLOCK_SIZE);
    }

    /*
    * 对于inode位图和数据块位图初始化还有些没弄清楚，后面再改一下
    */
}

//读数据块
char *read_block(int block_number)
{
    char *buf = (char *)malloc(1024);
    memset(buf, 0, 1024);
    disk_read_block(block_number * 2, buf);
    disk_read_block(block_number * 2 + 1, buf + DEVICE_BLOCK_SIZE);
    return buf;
}

//写数据块
void write_block(int block_number, char *buf, int size, int offset)
{
}

//获取超级块信息
void info_super_block(sp_block *sb)
{
    if (sb == NULL)
    {
        printf("Error:The Super Block is null.\n");
        return;
    }

    int32_t *head = (int32_t *)sb;
    printf(">>>>>>>>>>Super-Block-Information<<<<<<<<<<\n");

    if (sb->magic_num == MAGIC_NUM)
    {
        printf("File System:MyEXT2\n");
    }
    else
    {
        printf("File System:UnKnown, Magic Number:%d\n", sb->magic_num);
    }
    printf("Free block count:%d\n", sb->free_block_count);
    printf("Free inode count:%d\n", sb->free_inode_count);
    printf("Dir inode count:%d\n", sb->dir_inode_count);

    printf(">>>>>>>>>>Super-Block-Information<<<<<<<<<<\n");
}

//读取inode
inode *read_inode(int inode_number)
{
    int block_number = inode_number / 32 + 1; //找到inode对应的数据块 这里要注意的是第一个数据块已经被超级块占用了
    int block_offset = inode_number % 32;     //块内偏移
    char *read_inode_buf;
    inode *read_inode_node;
    read_inode_buf = read_block(block_number);
    read_inode_node = (inode *)(read_inode_buf + block_offset * 32);
    return read_inode_node;
}

//写入inode
void write_inode(inode *write_inode_node, int inode_number)
{
    int block_number = inode_number / 32 + 1; //找到inode对应的数据块 这里要注意的是第一个数据块已经被超级块占用了
    int block_offset = inode_number % 32;     //块内偏移
    write_block(block_number, (char *)write_inode_node, 32, block_offset * 32);
}

//创建inode
inode* create_inode(uint16_t file_type)
{
    inode* create_inode_node = (inode *)malloc(32); //为什么要用malloc 这个一开始遇到了什么问题 可以写在报告里面
    create_inode_node->file_type = file_type;
    create_inode_node->size = 0;
    create_inode_node->link = 1;
    for (int i = 0; i < 6; i++)
    {
        create_inode_node->block_point[i] = (uint32_t)0; 
    }
    return create_inode_node;
}

//创建目录项结构体
dir_item* create_dir_item(uint8_t type, uint32_t node_id, char *name)
{
    
}

/* 寻找空闲块
 * block_map[128]实际上记录的是128*32=4096个数据块的占用情况
 * 
*/
int find_free_block()
{
    //block_map[128]
    for (int i = 0; i < 128; i++)
    {
        for (int j = 0; j < 32; j++)
        {
            // >>右移
            // & 按位与
            if (((sb->block_map[i] >> j) & 1) == 0)
            {
                printf("New Free Block Found:%d\n", i * 32 + j);
                return i * 32 + j;
            }
        }
    }
}

//寻找空闲inode
int find_free_inode()
{
    //inode_map[128]
    for (int i = 0; i < 32; i++)
    {
        for (int j = 0; j < 32; j++)
        {
            // >>右移
            // & 按位与
            if (((sb->inode_map[i] >> j) & 1) == 0)
            {
                printf("New Free iNode Found:%d\n", i * 32 + j);
                return i * 32 + j;
            }
        }
    }
}

//进入文件夹
int ls(char *dir)
{
    //TODO
}

//创建文件夹
int mkdir(char *dirName)
{
    //TODO
}

//创建文件
int touch(char *fileName)
{
    //TODO
}

//复制文件
int cp(char *ori, char *dest)
{
    //TODO
}

// 关闭系统
void shutdown()
{
    close_disk();
}
