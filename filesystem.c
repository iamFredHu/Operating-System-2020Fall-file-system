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

    //read_super_block 读sb
    //读magic_num，判断是否为ext2文件系统，若是则打开文件系统，若不是则进行相应的初始化操作
    read_super_block(&sb);
    if (sb.magic_num == MAGIC_NUM)
    {
        //是ext2文件系统，所以可以打开文件系统，读取里面的内容
        printf("The information of the file system is now loading,please wait for a moment...\n");
        info_super_block(&sb);

        //读inode
        for (int i = 0; i < MAX_INODE_NUM; i++)
        {
            read_inode(&node[i], i + INODE_START);//这个地方还没想清楚 目前的想法是超级块占用前两个BLOCK_NUMBER (0、1) 而inode就从第三个block开始（编号为2）
        }

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
                printf("The information of the file system is now loading,please wait for a moment...\n");

                //超级块初始化
                init_super_block(&sb);
                write_super_block(&sb);
                info_super_block(&sb);

                //inode初始化
                for (int i = 0; i < MAX_INODE_NUM; i++)
                {
                    node[i].size = 0;              //初始化时尺寸为0
                    node[i].link = 1;              //在本系统中认为文件链接数为1
                    node[i].file_type = TYPE_FILE; //初始化时认为类型为FILE
                }

                //写inode
                for (int i = 0; i < MAX_INODE_NUM; i++)
                {
                    write_inode(&node[i], i + INODE_START);
                }

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
    sb->free_block_count = 4096 - 1 - 32; //空闲数据块数 超级块占用1 inode数组占用32 在这里用的是指导书里面的数据 uint32 32位 block_map[128] 32*128
    sb->free_inode_count = 1024;          //空闲inode数 在这里我们的节点数是32*32
    sb->dir_inode_count = 0;

    //数据块和inode位图初始化
    memset(sb->block_map, 0, sizeof(sb->block_map));
    memset(sb->inode_map, 0, sizeof(sb->inode_map));

    sb->inode_map[0] = MAX_NUM;
    sb->block_map[0] = ~(sb->block_map[0]);
    sb->block_map[1] = (1 << 31) | (1 << 30);
    /*
    对于inode位图和数据块位图初始化还有些没弄清楚，后面再改一下
    */
}

//读取超级块
void read_super_block(sp_block *sb)
{
    char buf[DEVICE_BLOCK_SIZE * 2];
    char *pointer;
    read_super_block_disk(buf);
    pointer = (char *)sb;
    for (int i = 0; i < sizeof(sb); i++)
    {
        *pointer = buf[i];
        pointer++;
    }
}

//写入超级块
void write_super_block(sp_block *sb)
{
    char buf[DEVICE_BLOCK_SIZE * 2];
    char *pointer;
    pointer = (char *)sb;
    for (int i = 0; i < sizeof(sb); i++)
    {
        buf[i] = *pointer;
        pointer++;
    }
    write_super_block_disk(buf);
}

/*
一开始没有想清楚超级块与buf之间的关系，按照系统默认的buf[512]来进行的设定，但是后来发现这样的话空间不够，所以
一个超级块必须对应两个buf[512]，即在超级块读写时，buf应该为[512*2]
*/

//在读取超级块时，从disk中读buf
void read_super_block_disk(char *buf)
{
    disk_read_block(0, buf);
    buf = buf + DEVICE_BLOCK_SIZE;
    disk_read_block(1, buf);
}

//在写入超级块时，将buf写入disk中
void write_super_block_disk(char *buf)
{
    disk_write_block(0, buf);
    buf = buf + DEVICE_BLOCK_SIZE;
    disk_write_block(1, buf);
}

//获取超级块信息
void info_super_block(sp_block *sb)
{
    if (sb == NULL)
    {
        fprintf(stderr, "Error:The Super Block is null.");
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
void read_inode(inode *node, int block_number)
{
    char buf[DEVICE_BLOCK_SIZE];
    disk_read_block(block_number, buf);
    //一个buf（diskblock）是512，而一个inode是32，因此要每次读取16个inode
    for (int i = 0; i < 16; i++)
    {
        node[i] = *(inode *)(buf + 32 * (i - 1));
    }
}

//写入inode
void write_inode(inode *node, int block_number)
{
    char buf[DEVICE_BLOCK_SIZE];
    //写入inode是不是应该反过来呢，每次读取16个inode，一次写入buf中
    for (int i = 0; i < 16; i++)
    {
        *buf = (char *)&(node[i]);
        *buf += 32;
    }
    disk_write_block(block_number, buf);
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
int cp()
{
    //TODO
}

// 关闭系统
void shutdown()
{
    close_disk();
}
