/*
文件系统的基本操作
*/

#include "filesystem.h"
#include "disk.c"

//文件系统初始化
void init_filesystem()
{
    //首先调用open_disk()函数打开磁盘
    if (open_disk() < 0)
    {
        printf("The disk can't be opened.\n");
    }
    else
    {
        printf("The disk has been successfully opened!\n");
    }

    char *init_filesystem_buf;
    init_filesystem_buf = read_block(0); //读block_number为0的块
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
                    int init_fs_index = i / 32;
                    int32_t init_fs_offset = i % 32;
                    sb->block_map[init_fs_index] = sb->block_map[init_fs_index] | (1 << init_fs_offset);
                    sb->free_block_count = sb->free_block_count - 1;
                    //superblock占用 magic_number 4B free_block_count 4B dir_inode_count 4B block_map 128*4B inode_map 32*4B 一共652B
                    //而data_block，即DEVICE_BLOCK_SIZE为512，所以要占用两个block
                    disk_write_block(0, (char *)sb);
                    disk_write_block(1, (char *)sb + DEVICE_BLOCK_SIZE);
                }
                //设置根目录项
                root_dir_node = mkdir("/");
                printf("The file system has been formatted,have a good time!\n");
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
    //初始化完毕后，或是已有信息，就可以进入文件系统了
    info_super_block(sb);

    //从inode number为0的地方读取根目录inode，所以上面格式化的时候就要设置一个根目录的inode
    root_dir_node = read_inode(0);
    //根目录dir_item初始化
    root_dir_dir_item = init_dir_item(TYPE_DIR, 0, "/");
    //目录栈初始化
    path_t.top_item = -1;
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

void push_dir_item(dir_item *push_dir_item_dir_item)
{
    path_t.top_item = path_t.top_item + 1;
    //目录长度过长判断（预设）
    /*
    if(path_t.top_item > 19)
    {
        printf("ERROR:The path is too long.\n");
        return -1;
    }
     */
    path_t.dir_item_stack[path_t.top_item].inode_id = push_dir_item_dir_item->inode_id;
    memmove(path_t.dir_item_stack[path_t.top_item].name, push_dir_item_dir_item->name, 121);
    path_t.dir_item_stack[path_t.top_item].type = push_dir_item_dir_item->type;
    path_t.dir_item_stack[path_t.top_item].valid = push_dir_item_dir_item->valid;
}

dir_item *pop_dir_item()
{
    //为空
    if (path_t.top_item == -1)
    {
        return (dir_item *)0;
    }
    dir_item *pop_dir_item_dir_item = &path_t.dir_item_stack[path_t.top_item];
    path_t.top_item = path_t.top_item - 1;
    return pop_dir_item_dir_item;
}

dir_item *top_dir_item(int number)
{
    if (path_t.top_item == -1)
    {
        return (dir_item *)0;
    }

    if (path_t.top_item - number >= 0)
    {
        return &path_t.dir_item_stack[path_t.top_item - number];
    }
    else
    {
        return &path_t.dir_item_stack[0];
    }
}

//处理路径
char *transfer_path(char **path)
{
    char *pathString;
    char *endString;

    pathString = *path;
    if (pathString[0] == '/')
    {
        pathString = pathString + 1;
    }
    endString = pathString;
    while (*endString != '/' && *endString != '\0')
    {
        endString = endString + 1;
    }

    //求字符串长度
    int string_len = endString - pathString;
    char *string = (char *)malloc(string_len);
    for (int i = 0; i < string_len; i++)
    {
        string[i] = pathString[i];
    }
    *path = endString;
    return string;
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

//已知inode，找到dir_item的名字
dir_item *search_dir_item_in_inode(inode *dir_node, char *dir_name, int *block_index, int *block_offset, int flag)
{
    int dir_item_number = dir_node->size / 128;
    *block_offset = 0;
    for (int i = 0; i < 6; i++)
    {
        if (dir_node->block_point[i] != 0)
        {
            dir_item_number = dir_item_number - 8;
            int dir_item_per_block;
            int last_flag;
            if (dir_item_number >= 0)
            {
                dir_item_per_block = 8;
            }
            else
            {
                dir_item_per_block = dir_item_number + 8;
            }
            if (dir_item_number < 0)
            {
                last_flag = 1;
            }
            else
            {
                last_flag = 0;
            }
            for (int j = 0; j < dir_item_per_block; j++)
            {
                dir_item *sdiii_dir_item = read_dir_item(dir_node, i, j);
                if (sdiii_dir_item->valid && flag)
                {
                    printf("%s %s\n", sdiii_dir_item->name, sdiii_dir_item->type == TYPE_FILE ? "\e[35;1mFILE\e[0m" : "\e[36;1mDIR\e[0m");
                }
                if (strcmp(sdiii_dir_item->name, dir_name) == 0)
                {
                    *block_index = i;
                    *block_offset = 128 * j;
                    return sdiii_dir_item;
                }
            }
            *block_offset = *block_offset + 128 * dir_item_per_block;
            if (last_flag)
            {
                *block_index = i;
            }
        }
    }
    return (dir_item *)0;
}

//查找目录
int search_dir_item(char *path, char **dir_name, dir_item **current_dir_item, dir_item **higher_level_dir_item, int flag)
{
    //，如果是根目录的话
    if (!strcmp(path, "/"))
    {
        *higher_level_dir_item = root_dir_dir_item;
        *current_dir_item = root_dir_dir_item;
        *dir_name = "/";
        //如果是根目录，上级目录和当前目录的dir_item都是根目录的dir_item
        if (flag)
        {
            path_t.top_item = -1;
            push_dir_item(root_dir_dir_item);
        }
        return 0;
    }

    //如果目录是..（按指导书中说明是上级目录）
    if (path[0] == '.' && path[1] == '.')
    {
        *current_dir_item = top_dir_item(1);
        transfer_path(&path);
        //int memcmp(const void *str1, const void *str2, size_t n));
        //如果返回值 < 0，则表示 str1 小于 str2
        //如果返回值 > 0，则表示 str2 小于 str1
        int memcmp_flag = memcmp(top_dir_item(0)->name, "/", sizeof("/"));
        if (flag && memcmp_flag != 0)
        {
            pop_dir_item();
        }
    }
    else if (path[0] == '/')
    {
        *current_dir_item = root_dir_dir_item;
        if (flag)
        {
            path_t.top_item = -1;
            push_dir_item(root_dir_dir_item);
        }
    }
    else if (path[0] == '.')
    {
        transfer_path(&path);
        *current_dir_item = top_dir_item(0);
    }
    else
    {
        *current_dir_item = top_dir_item(0);
    }

    while (*dir_name = transfer_path(&path))
    {
        if (strlen(*dir_name) == 0)
            break;
        //测试信息 待删除
        printf("TEST INFORMATION: find_dir_item() path:%s dir_name:%s, len:%ld\n", path, *dir_name, strlen(*dir_name));
        int block_index = 0;
        int block_offset = 0;
        int current_dir_item_inode_id = (*current_dir_item)->inode_id;
        inode *current_dir_node = read_inode(current_dir_item_inode_id);
        if (current_dir_node->file_type == TYPE_FILE)
        {
            printf("ERROR INFORMATION: FILE TYPE ERROR, NOT A DIR\n");
            return -1;
        }
        dir_item *lower_level_dir_item = search_dir_item_in_inode(current_dir_node, *dir_name, &block_index, &block_offset, 0);
        if (!lower_level_dir_item)
        {
            //测试信息 待删除
            printf("TEST INFORMATION: %s NOT EXIST\n", *dir_name);
            return -1;
        }
        *higher_level_dir_item = *current_dir_item;
        *current_dir_item = lower_level_dir_item;
        if (lower_level_dir_item->type == TYPE_FILE)
        {
            //测试信息 待删除
            printf("TEST INFORMATION: %s IS A FILE\n", *dir_name);
            return 0;
        }
        if (flag)
        {
            push_dir_item(lower_level_dir_item);
        }
    }
    return 0;
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
inode *mkdir(char *path)
{
    //创建文件夹首先要改变sb.dir_inode_count(目录inode数)
    sb->dir_inode_count = sb->dir_inode_count + 1;
    //然后是区分根目录和非根目录，根据传入的dirName进行判断
    //这里用到strcmp()函数：比较两个字符串 如果返回值 = 0，则表示 str1 等于 str2
    //根目录
    if (strcmp(path, "/") == 0)
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
        /* 对于不是根目录的目录来说 
         * 1、要找到上一级目录的dir_item以及inode，然后创建一个新的dir_item
         * 2、更新inode和dir_item
         */

        //创建目录分为目录存在或不存在两种可能，所以需要调用函数进行判断
        dir_item *current_dir_item;
        dir_item *higher_level_dir_item;
        inode *current_inode;
        inode *lower_level_inode;
        char *dir_name;
        printf("TEST INFORMATION: path %s\n", path);
        int search_result = search_dir_item(path, &dir_name, &current_dir_item, &higher_level_dir_item, 0);
        if (search_result < 0)
        {
            //测试信息
            printf("TEST INFORMATION: PATH NOT FOUND NOW CREATE %s\n", path);
            printf("TEST INFORMATION: search return %d\n",search_result);
            //标记一下 这里开始出bug
            //因为目录不存在，所以进行创建
            lower_level_inode = init_inode(TYPE_DIR);
            uint32_t inode_index = create_inode(lower_level_inode);
            dir_item *new_dir_dir_item = init_dir_item(TYPE_DIR, inode_index, dir_name);
            printf("TEST1\n");
            //创建.和..的目录 分别表示当前目录和上级目录
            write_dir_item(init_dir_item(TYPE_DIR, new_dir_dir_item->inode_id, "."), lower_level_inode);
            write_dir_item(init_dir_item(TYPE_DIR, current_dir_item->inode_id, ".."), lower_level_inode);
            write_inode(lower_level_inode, new_dir_dir_item->inode_id);
            printf("TEST2\n");
            current_inode = read_inode(current_dir_item->inode_id);
            write_dir_item(new_dir_dir_item, current_inode);
            write_inode(current_inode, current_dir_item->inode_id);
            printf("TEST3\n");
            lower_level_inode = mkdir(path);
            disk_write_block(0, (char *)sb);
            disk_write_block(1, (char *)sb + DEVICE_BLOCK_SIZE);
            return lower_level_inode;
        }
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
