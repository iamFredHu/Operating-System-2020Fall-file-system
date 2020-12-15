/*
文件系统的基本操作
*/

#include "filesystem.h"
#include "disk.c"

//文件系统初始化
void init_filesystem()
{

    //调用open_disk函数
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
    if (sb->magic_num == MAGIC_NUM)
    {
        //是ext2文件系统，所以可以打开文件系统，读取里面的内容
        printf("The information of the file system is now loading,please wait for a moment...\n");
        info_super_block(&sb);
        for (int i = 0; i < 128; i++)
        {
            //TODO 读取inode
        }
        printf("The file system has been opened...\n");
    }
    else
    {
        int format_flag = 1;
        while (format_flag)
        {
            printf("The file system is unknown,format the disk and rebuild a new ext2 file system?(y/n)");
            char input_choice = getchar();
            if (input_choice == 'y')
            {
                //进行文件系统初始化
                init_super_block(&sb);
                printf("The information of the file system is now loading,please wait for a moment...\n");
                info_super_block(&sb);
                write_super_block(&sb);
                //TODO iNode 初始化和写操作
                printf("Formating finished,good luck and have fun!\n");
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
}

//读取超级块
void read_super_block(sp_block *sb)
{
    char buf[DEVICE_BLOCK_SIZE];
    disk_read_block(0, buf);
    /*阅读了一下disk.c中这个函数的源代码
    fread(buf, DEVICE_BLOCK_SIZE, 1, disk)
    C 库函数 size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream) 从给定流 stream 读取数据到 ptr 所指向的数组中
    所以这里要做的事是，把从disk_read_block函数中读到的buf分析出来
    但是buf这个是以数组的形式存在的，里面存放的是ascii码，我们要做的就是写一个atoi函数
    */
    sb->magic_num = string_asciitoint(&buf[0], 4);        //magic_num 属于int32_t 32bit -- 4Bytes
    sb->free_block_count = string_asciitoint(&buf[4], 4); //free_block_count 属于int32_t 32bit -- 4Bytes
    sb->free_inode_count = string_asciitoint(&buf[8], 4); //free_inode_count 属于int32_t 32bit -- 4Bytes
    sb->dir_inode_count = string_asciitoint(&buf[12], 4); //dir_inode_count 属于int32_t 32bit -- 4Bytes

    for (int i = 0; i < 128; i++)
    {
        sb->block_map[i] = string_asciitoint(&buf[16 + i], 1); //FLAG 标记一下这个地方感觉有点问题 明天把它改了
    }

    for (int i = 0; i < 32; i++)
    {
        sb->inode_map[i] = string_asciitoint(&buf[144 + i], 1); //FLAG 标记一下这个地方感觉有点问题 明天把它改了
    }
    /*按照指导书上的示例
    magic_num 4B 
    free_block_count 4B 
    free_inode_count 4B
    dir_inode_count 4B
    block_map[128] ?
    inode_map[32] ?

    Q:一点疑问 buf大小为512 这样感觉好像超出界限了？？ -12.15留
    */

    printf("Read Super Block:%s\n", buf); //调试用函数 原谅我太菜了只能一直printf
}

//写入超级块
void write_super_block(sp_block *sb)
{
    char buf[DEVICE_BLOCK_SIZE];

    /*
    写的时候和前面读相反
    fwrite(buf, DEVICE_BLOCK_SIZE, 1, disk)
    把buf写入disk中
    所以我们要把int型的信息转成ascii，然后再存入buf中
    */

    string_intasciitoint(&buf[0], sb->magic_num);
    string_intasciitoint(&buf[4], sb->free_block_count);
    string_intasciitoint(&buf[8], sb->free_inode_count);
    string_intasciitoint(&buf[12], sb->dir_inode_count);

    for (int i = 0; i < 128; i++)
    {
        string_intasciitoint(&buf[16], sb->block_map[i]);
    }

    for (int i = 0; i < 32; i++)
    {
        string_intasciitoint(&buf[144], sb->inode_map[i]);
    }

    disk_write_block(0, buf);
    printf("Write Super Block:%s\n", buf); //调试用函数 原谅我太菜了只能一直printf
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

//Ascii to INT
int string_asciitoint(char *string, int length)
{
    int number_int = 0;
    for (int i = 0; i < length; i++)
    {
        if (!isdigit(string[i]))
            return number_int;
        number_int = 10 * number_int + (string[i] - '0');
    }
    //这个返回的number是指将ascii码转换成int类型后的数字 例如如果输入'123'则返回的number_int值为123
    return number_int;
}

//INT to Ascii
void string_intasciitoint(char *string, int number_int)
{
    char temp[20];
    int i, j;
    i = 0;

    while ((number_int /= 10) > 0)
    {
        temp[i++] = number_int % 10 + '0'; //把int转成ascii
    }

    j = 0;

    while (i)
    {
        string[j++] = temp[--i];
    }

    string[j] = '\0';
}

int main()
{
    init_filesystem();
}