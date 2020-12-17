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
    char *init_fs_buf;
    init_fs_buf = read_block(0);
    sb = (sp_block *)init_fs_buf;
    if (sb->magic_num == MAGIC_NUM)
    {
        //是ext2文件系统，所以可以打开文件系统，读取里面的内容
        info_super_block(sb);

        //读取根目录
        root_dir_node = read_inode(0);
        root_dir_dir_item = init_dir_item(TYPE_DIR, 0, "/");

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
                disk_write_block(0, (char *)sb);
                disk_write_block(1, (char *)sb + DEVICE_BLOCK_SIZE);
                info_super_block(sb);

                //根目录初始化
                //格式化这里首先要创建根目录
                root_dir_node = mkdir("/");
                root_dir_node = read_inode(0);
                root_dir_dir_item = init_dir_item(TYPE_DIR, 0, "/");

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
void init_super_block(sp_block *init_super_block_sb)
{
    init_super_block_sb->magic_num = MAGIC_NUM;
    init_super_block_sb->free_block_count = 4096 - 1 - 32; //空闲数据块数
    init_super_block_sb->free_inode_count = 1024;          //空闲inode数 在这里我们的节点数是32*32
    init_super_block_sb->dir_inode_count = 0;

    /*
    * 有关数据块大小的计算
    * 示例中一共可以表示128*32=4096个数据块 其中超级块和inode也需要占用数据块
    * 每个数据块的大小为1KiB
    * 超级块占用1个数据块 inode占用 32 * 1024(1024个inode，每个inode占用32B空间) / 1024 KiB  
    */

    //数据块和inode位图初始化
    memset(init_super_block_sb->block_map, 0, sizeof(init_super_block_sb->block_map));
    memset(init_super_block_sb->inode_map, 0, sizeof(init_super_block_sb->inode_map));

    //表明数据块已占用， 0-super bock 1-32 inode
    for (int i = 0; i < 33; i++)
    {
        //对block_map按位操作
        int index = i / 32;
        int32_t offset = i % 32;
        uint32_t block_map_process = 1 << offset;
        init_super_block_sb->block_map[index] |= block_map_process;
        init_super_block_sb->free_block_count = init_super_block_sb->free_block_count - 1;
        disk_write_block(0, (char *)init_super_block_sb);
        disk_write_block(1, (char *)init_super_block_sb + DEVICE_BLOCK_SIZE);
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
    //分配数据块
    int index = block_number / 32;
    int32_t write_block_offset = block_number % 32;
    uint32_t block_map_process = 1 << write_block_offset;
    sb->block_map[index] |= block_map_process;
    sb->free_block_count = sb->free_block_count - 1;
    disk_write_block(0, (char *)sb);
    disk_write_block(1, (char *)sb + DEVICE_BLOCK_SIZE);

    char *temp_buf = read_block(block_number);
    //由buf所指内存区域复制size个字节到temp_buf+offset所指内存区域
    memmove(temp_buf + offset, buf, size);
    disk_write_block(block_number * 2, temp_buf);
    disk_write_block(block_number * 2 + 1, temp_buf + DEVICE_BLOCK_SIZE);
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

//初始化inode
inode *init_inode(uint16_t file_type)
{
    inode *create_inode_node = (inode *)malloc(32); //为什么要用malloc 这个一开始遇到了什么问题 可以写在报告里面
    create_inode_node->file_type = file_type;
    create_inode_node->size = 0;
    create_inode_node->link = 1;
    for (int i = 0; i < 6; i++)
    {
        create_inode_node->block_point[i] = (uint32_t)0;
    }
    return create_inode_node;
}

//创建inode
uint32_t create_inode(inode *create_inode_node)
{
    //找到一个空闲的inode
    uint32_t create_inode_index = find_free_inode();
    int block_number = create_inode_index / 32 + 1; //找到数据块，注意这里第一个数据块是超级块
    int block_offset = create_inode_index % 32;     //数据块偏移

    //分配inode
    int alloc_inode_index = create_inode_index / 32;
    int32_t alloc_inode_offset = create_inode_index / 32;
    uint32_t inode_map_process = 1 << alloc_inode_offset;
    sb->inode_map[alloc_inode_index] = sb->inode_map[alloc_inode_index] | inode_map_process;
    sb->free_inode_count = sb->free_inode_count - 1;
    disk_write_block(0, (char *)sb);
    disk_write_block(1, (char *)sb + DEVICE_BLOCK_SIZE);

    write_block(block_number, (char *)create_inode_node, 32, block_offset * 32);
    return create_inode_index;
}

//创建目录项结构体
dir_item *init_dir_item(uint8_t type, uint32_t inode_id, char *name)
{
    dir_item *create_dir_item_diritem = (dir_item *)malloc(128);
    create_dir_item_diritem->inode_id = inode_id;
    create_dir_item_diritem->valid = 1;
    create_dir_item_diritem->type = type;
    for (int i = 0; i < 121; i++)
    {
        create_dir_item_diritem->name[i] = name[i];
    }
    return create_dir_item_diritem;
}

//写目录
void write_dir_item(dir_item *dir, inode *read_dir_item_node)
{
    int block_number = 0;
    //首先要读inode中的file_type 如果是FILE则不能创建
    if (read_dir_item_node->file_type = TYPE_FILE)
        printf("Wrong Node File Type");
    int size = read_dir_item_node->size;
    int block_point_index = size / 1024;
    int block_offset = size % 1024;
    if (block_point_index > 6)
    {
        printf("Block Point Index Error!\n");
        return;
    }
    if (read_dir_item_node->block_point[block_point_index] == 0)
    {
        /* size是从inode中读到的文件大小，根据size可以求出数据块指针的index
         * 如果数据块的索引值是0的话，就说明需要分配一个数据块，如果不为0，就根据这个索引值找到对应的数据块
         */
        block_number = find_free_block();
        read_dir_item_node->block_point[block_point_index] = block_number;
    }
    else
    {
        block_number = read_dir_item_node->block_point[block_point_index];
    }
    write_block(block_number, (char *)dir, 128, block_offset);
    read_dir_item_node->size = read_dir_item_node->size + 128;
}

//读目录
dir_item *read_dir_item(inode *read_dir_item_node, int block_point_index, int offset_index)
{
    char *read_dir_item_buf = read_block(read_dir_item_node->block_point[block_point_index]);
    return (dir_item *)(read_dir_item_buf + offset_index * 128);
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
inode *mkdir(char *dirName)
{
    //创建文件夹首先要改变sb.dir_inode_count(目录inode数)
    sb->dir_inode_count = sb->dir_inode_count + 1;
    //然后是区分根目录和非根目录，根据传入的dirName进行判断
    //这里用到strcmp()函数：比较两个字符串 如果返回值 = 0，则表示 str1 等于 str2
    //根目录
    if (strcmp(dirName, "/") == 0)
    {
        //给新建的目录进行inode初始化
        inode *mkdir_node = init_inode(TYPE_DIR);
        //如果之前已经有根目录了，则不能再创建新的根目录，把这里的信息返回给shell
        if (root_dir_node)
        {
            printf("The root dir has already existed.\n");
            return 0;
        }
        dir_item *current_dir_item = init_dir_item(TYPE_DIR, 0, ".");
        dir_item *higher_level_dir_item = init_dir_item(TYPE_DIR, 0, "..");
        write_dir_item(current_dir_item, mkdir_node);
        write_dir_item(higher_level_dir_item, mkdir_node);
        create_inode(mkdir_node);
        disk_write_block(0, (char *)sb);
        disk_write_block(1, (char *)sb + DEVICE_BLOCK_SIZE);
        return mkdir_node;
    }
    //不是根目录
    else
    {
        /* 对于不是根目录的目录来说 明天再做吧 今天好困
         *
         */
    }
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
