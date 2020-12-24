#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "disk.c"
#include "utils.c"
#include "file_system.h"
#include "inode.h"

//dir_item 初始化，即给定相关参数 初始化一个dir_item
dir_item *init_dir_item(uint8_t type, uint32_t inode_id, char *name)
{
    //新建一个临时dir_item
    dir_item *init_dir_item_dir_item = (dir_item *)malloc(128);
    //根据函数参数设置inode_id、type、valid、name信息
    init_dir_item_dir_item->inode_id = inode_id;
    init_dir_item_dir_item->type = type;
    init_dir_item_dir_item->valid = 1;
    memmove(init_dir_item_dir_item->name, name, strlen(name));

    return init_dir_item_dir_item;
}

//inode 初始化，即给定相关参数 初始化一个inode
inode *init_inode(uint16_t file_type)
{
    //新建一个临时inode
    inode *init_inode_inode = (inode *)malloc(32);
    //根据函数参数设置file_type、size、link、block_point信息
    init_inode_inode->file_type = file_type;
    init_inode_inode->size = 0;
    init_inode_inode->link = 1;
    for (int i = 0; i < 6; i++)
    {
        init_inode_inode->block_point[i] = 0;
    }
    return init_inode_inode;
}

dir_item *top(int n)
{
    if (path_stack.top == -1 ? 1 : 0)
    {
        return (struct dir_item *)0;
    }

    return path_stack.top - n >= 0 ? &path_stack.stack[path_stack.top - n] : &path_stack.stack[0];
}

dir_item *pop()
{
    if (path_stack.top == -1 ? 1 : 0)
    {
        return (struct dir_item *)0;
    }
    struct dir_item *item = &path_stack.stack[path_stack.top];
    path_stack.top--;
    return item;
}

int push(struct dir_item *dir_item)
{
    path_stack.top++;
    if (path_stack.top > 19)
    {
        printf("push path() path_stack is too long");
        return -1;
    }
    path_stack.stack[path_stack.top].inode_id = dir_item->inode_id;
    memmove(path_stack.stack[path_stack.top].name, dir_item->name, 121);
    path_stack.stack[path_stack.top].type = dir_item->type;
    path_stack.stack[path_stack.top].valid = dir_item->valid;
    return 0;
}

//找空闲的block（数据块）
/* 如何找空闲的block呢？
 * 根据指导书，我们有block_map的每一比特记录了数据块的占用情况，在指导书的示例中，可以表示128*32=4096个数据块的占用情况
 * 所以我们将block_map按位与，如果结果为0，则说明对应的block为空闲的
 * 返回block_map位图第多少个比特为空闲
 */
uint32_t search_free_block()
{
    //i是block_map的index，j是一个uint32_t占用的每一bit(共32bit)
    for (uint32_t i = 0; i < 128; i++)
    {
        for (uint32_t j = 0; j < 32; j++)
        {
            if (((sb->block_map[i] >> j) & 1) == 0)
            {
                return i * 32 + j;
            }
        }
    }
}

//找空闲的inode
/* 如何找空闲的inode呢？
 * 根据指导书，我们有inode_map的每一比特记录了数据块的占用情况，在指导书的示例中，可以表示32*32=1024个inode的占用情况
 * 所以我们将inode_map按位与，如果结果为0，则说明对应的inode为空闲的
 * 返回inode_map位图第多少个比特为空闲
 */
uint32_t get_free_inode()
{
    for (uint32_t i = 0; i < 32; i++)
    {
        for (uint32_t j = 0; j < 32; j++)
        {
            if (((sb->inode_map[i] >> j) & 1) == 0)
            {
                printf("find one inode: %d\n", i * 32 + j);
                return i * 32 + j;
            }
        }
    }
}

//分配inode
void alloc_inode(uint32_t inode_number)
{
    //inode_number是指inode编号 如果要根据inode编号分配inode，要修改inode_map
    int inode_map_index = inode_number / 32;
    int32_t bit_index = inode_number % 32;
    //inode_map 32*32 inode_map_index是inode_map[]的index bit_index是uint32_t每一bit的index
    sb->inode_map[inode_map_index] |= 1 << bit_index;
    //同时要减少空闲inode数
    sb->free_inode_count--;
    //写入磁盘
    disk_write_block(0, (char *)sb);
    disk_write_block(1, (char *)sb + DEVICE_BLOCK_SIZE);
}

//将inode写入磁盘
uint32_t write_inode(struct inode *inode)
{
    //首先找到空闲的inode
    uint32_t free_inode_index = get_free_inode();
    //数据块id //TODO 为什么要+1
    int block_id = free_inode_index / 32 + 1;
    //位图比特id
    int bit_index = free_inode_index % 32;
    //分配inode
    alloc_inode(free_inode_index);
    //写入磁盘
    write_block(block_id, (char *)inode, 32, bit_index * 32);
    return free_inode_index;
}

/**
 * 
 * inode_num：要更新的inode编号
 * update_inode：要更新的inode
 */
int sync_inode(uint32_t inode_id, struct inode *update_inode)
{
    int block_id = inode_id / NINODE_PER_BLK + 1; //第几个数据块
    int bit_index = inode_id % NINODE_PER_BLK;    //数据块内偏移
    write_block(block_id, (char *)update_inode, INODESZ, bit_index * INODESZ);
}

//根据inode_id,返回相应的inode地址
inode *read_inode(uint32_t inode_id)
{
    //数据块id 需要注意第一个数据块id被超级块占用，所以需要—+1
    int block_id = (inode_id / 32) + 1;
    //数据块内偏移量，一个inode占用大小为32B，一个数据块大小为512B
    int bit_index = inode_id % 32;

    //根据数据块id读数据块，然后再把读到的buf强转为inode
    char *read_inode_buf;
    inode *read_inode_node;
    read_inode_buf = read_block(block_id);
    //注意加上偏移量
    read_inode_node = (inode *)(read_inode_buf + bit_index * 32);

    return read_inode_node;
}

//数据块的分配
void alloc_block(uint32_t block_id)
{
    sb->block_map[block_id / 32] = sb->block_map[block_id / 32] | (1 << block_id % 32);
    //每分配一次，空闲的数据块数量减少一个
    sb->free_block_count--;
    //写入磁盘，注意超级块占两个块
    disk_write_block(0, (char *)sb);
    disk_write_block(1, (char *)sb + DEVICE_BLOCK_SIZE);
}

//根据block_id从磁盘中读对应的块
char *read_block(uint32_t block_id)
{
    //设置一个临时的buf，分配内存空间并赋初值0
    char *read_block_buf = (char *)malloc(1024);
    memset(read_block_buf, 0, 1024);
    //注意DEVICE_BLOCK_SIZE为512B 而数据块的大小为1024B,是两倍的关系
    disk_read_block(block_id * 2, read_block_buf);
    disk_read_block(block_id * 2 + 1, read_block_buf + DEVICE_BLOCK_SIZE);
    return read_block_buf;
}

//写数据块
int write_block(uint32_t block_id, char *buf, int size, int offset)
{
    //首先要进行数据块的分配
    sb->block_map[block_id / 32] = sb->block_map[block_id / 32] | (1 << block_id % 32);
    sb->free_block_count = sb->free_block_count - 1;
    disk_write_block(0, (char *)sb);
    disk_write_block(1, (char *)sb + DEVICE_BLOCK_SIZE);

    char *write_block_buf = read_block(block_id);
    memmove(write_block_buf + offset, buf, size);
    disk_write_block(block_id * 2, write_block_buf);
    disk_write_block(block_id * 2 + 1, write_block_buf + DEVICE_BLOCK_SIZE);
    return 0;
}

/* dir_item相关 */
int create_dir_item(struct inode *dir_node, struct dir_item *dir_item)
{
    if (dir_node->file_type == TYPE_FILE)
        return -1;
    int sz = dir_node->size;
    int blk_index = sz / 1024;
    int blk_off = sz % 1024;
    if (blk_index > 6)
    {
        printf("create_dir_item() can't alloc more data block \n");
        return -1;
    }
    int block_id = 0;
    if (dir_node->block_point[blk_index] == 0)
    {
        block_id = search_free_block();
        dir_node->block_point[blk_index] = block_id;
    }
    else
    {
        block_id = dir_node->block_point[blk_index];
    }
    write_block(block_id, (char *)dir_item, 128, blk_off);
    dir_node->size += 128;
    return 0;
}

dir_item *read_dir_item(struct inode *dir_node, int blk_index, int off_index)
{
    if (off_index >= 8)
    {
        printf("read_dir_item() cannot read num greater than 8 \n");
        return 0;
    }
    char *buf = read_block(dir_node->block_point[blk_index]);
    return (struct dir_item *)(buf + off_index * 128);
}

/**
 * 
 * 在制定inode下寻找dir_item，仅需名字即可
 * 
 * footprint：代表需不需要输出查找过程
 * 
 * 
 * 
 * 目前暂时不允许文件夹与文件重名
 */
dir_item *search_dir_item_by_inode(struct inode *dir_inode, char *dir_name, int *blk_index, int *blk_off, int footprint)
{
    int total_dir_item = dir_inode->size / 128;
    *blk_off = 0;
    for (int i = 0; i < 6; i++)
    {
        if (dir_inode->block_point[i] != 0)
        {
            total_dir_item -= 8;
            int dir_item_per_blk = total_dir_item >= 0 ? 8 : total_dir_item + 8;
            int is_last = total_dir_item < 0 ? 1 : 0;
            for (int j = 0; j < dir_item_per_blk; j++)
            {
                struct dir_item *dir_item = read_dir_item(dir_inode, i, j);
                if (dir_item->valid && footprint)
                {
                    printf("%s %s\n", dir_item->name, dir_item->type == TYPE_FILE ? "\e[35;1mFILE\e[0m" : "\e[36;1mDIR\e[0m");
                }
                if (strcmp(dir_item->name, dir_name) == 0)
                {
                    *blk_index = i;
                    *blk_off = j * 128;
                    return dir_item;
                }
            }
            *blk_off += dir_item_per_blk * 128;
            if (is_last)
            {
                *blk_index = i;
            }
        }
    }
    return (struct dir_item *)0;
}

/**
 * 根据path，找到相应的dir_item
 * ../adadad/adad
 * ./adad/ad
 * addd/add
 * 
 * /dad/ad
 * 
 * path: 路径
 * dir_name：如果没有
 * current_dir_item:用于返回找到的dir_item
 * last_dir_item：用于返回dir_item的上一级dir_item
 * is_follow：是否跟踪路径
 * 
 * return：
 * 没有找到返回-1，否则返回0
 */
int search_dir_item_by_path(char *path, char **dir_name, struct dir_item **current_dir_item, struct dir_item **up_dir_item, int is_follow)
{
    if (strcmp(path, "/") == 0)
    {
        /* / */
        *current_dir_item = root_dir_item;
        *up_dir_item = root_dir_item;
        *dir_name = "/";
        if (is_follow)
        {
            path_stack.top = -1;
            push(root_dir_item);
        }
        return 0;
    }
    //if (path[0] == '.' && path[1] == '.')
    //{
    /* ../awdawdawd/awdaw */
    //get_last_dir_item()
    //*current_dir_item = top(1);
    //peek_path(&path);
    //if (is_follow && (memcmp(top(0)->name, "/", sizeof("/")) != 0))
    //pop();
    //}
    else if (path[0] == '/')
    {
        /* /dawdawd/awdaw */
        *current_dir_item = root_dir_item;
        if (is_follow)
        {
            path_stack.top = -1;
            push(root_dir_item);
        }
    }
    /* ./adad/adad */
    //else if (path[0] == '.')
    //{
    //peek_path(&path);
    //get_current_dir_item
    //*current_dir_item = top(0);
    //}
    /* asdad/adad */
    else
    {
        //get_current_dir_item
        *current_dir_item = top(0);
    }

    while (*dir_name = peek_path(&path))
    {
        if (strlen(*dir_name) == 0)
        {
            break;
        }
        printf("search_dir_item_by_path() path %s, dirname %s , len %ld\n", path, *dir_name, strlen(*dir_name));
        int blk_index = 0;
        int block_off = 0;
        struct inode *current_dir_node = read_inode((*current_dir_item)->inode_id);
        if (current_dir_node->file_type == TYPE_FILE)
        {
            printf("find_inode() error: %s is not directory \n", (*current_dir_item)->name);
            return -1;
        }
        struct dir_item *next_dir_item =
            search_dir_item_by_inode(current_dir_node, *dir_name, &blk_index, &block_off, 0);
        if (!next_dir_item)
        {
            printf("find_inode() directory %s is not exist!\n", *dir_name);
            return -1;
        }
        *up_dir_item = *current_dir_item;
        *current_dir_item = next_dir_item;
        if (next_dir_item->type == TYPE_FILE)
        {
            printf("find_inde() %s is a file , stop here!\n", *dir_name);
            return 0;
        }
        if (is_follow)
            push(next_dir_item);
    }
    return 0;
}
/********************** Codes *************************/
void shutdown_cmd()
{
    close_disk();
    exit(0);
}

//进入目录
//输入 *path 路径
void ls_cmd(char *path)
{
    //目录名
    char *dir_name;
    //当前目录的dir_item
    struct dir_item *cur_dir_item;
    //当前目录的inode
    struct inode *cur_inode;
    //上级目录的dir_item
    struct dir_item *up_dir_item;
    //根据path找到对应的dir_item
    search_dir_item_by_path(path, &dir_name, &cur_dir_item, &up_dir_item, 0);
    //根据当前目录的dir_item，有对应的inode_id，根据inode_id可以找到对应的inode
    cur_inode = read_inode(cur_dir_item->inode_id);
    int block_index = 0;
    int block_offset = 0;
    //根据inode查找相应的dir_item
    search_dir_item_by_inode(cur_inode, "/", &block_index, &block_offset, 1);
}

//创建文件
//输入 *path 路径
inode *touch_cmd(char *path)
{
    struct dir_item *cur_dir_item;
    struct dir_item *up_dir_item;
    struct inode *cur_inode;
    struct inode *down_inode;
    char *dir_name;
    char *file_name = get_file_name(path);
    printf("path %s \n", path);
    if (search_dir_item_by_path(path, &dir_name, &cur_dir_item, &up_dir_item, 0) < 0)
    {
        if (memcmp(dir_name, file_name, strlen(file_name)) != 0)
        {
            printf("mkdir_cmd() directory %s is not exist!\n", dir_name);
            return (struct inode *)0;
        }

        printf("mkdir_cmd() not find path: %s\n", path);
        printf("mkdir_cmd() now we'll create one for you !\n");
        /* 创建不存在的目录 */
        struct inode *down_inode = init_inode(TYPE_FILE);
        uint32_t inode_index = write_inode(down_inode);
        struct dir_item *dir_item = init_dir_item(TYPE_FILE, inode_index, dir_name);
        /* 刷新当前节点 */
        cur_inode = read_inode(cur_dir_item->inode_id);
        create_dir_item(cur_inode, dir_item);
        sync_inode(cur_dir_item->inode_id, cur_inode);

        return down_inode;
    }
    printf("touch_cmd() file %s is already exists !\n", file_name);
}

inode *mkdir_cmd(char *path)
{
    sb->dir_inode_count++;
    /* 根目录 */
    if (strcmp(path, "/") == 0)
    {
        struct inode *dir_node = init_inode(TYPE_DIR);
        if (root_dir_node)
        {
            printf("mkdir_cmd() can not create another root directory! \n");
            return (struct inode *)0;
        }
        write_inode(dir_node);
        disk_write_block(0, (char *)sb);
        disk_write_block(1, (char *)sb + DEVICE_BLOCK_SIZE);
        return dir_node;
    }
    /* 非根目录 */
    else
    {
        /**
         * TODO
         * 
         * 基本逻辑：
         * 找到上一级dir_item，顺便找到它的inode，为其创建dir_item，
         * 
         * 记得每次更新 inode 和 
         */
        /* 测试，创建一个DIR */
        struct dir_item *current_dir_item;
        struct dir_item *up_dir_item;
        struct inode *current_inode;
        struct inode *down_inode;
        char *dir_name;
        printf("path %s \n", path);
        if (search_dir_item_by_path(path, &dir_name, &current_dir_item, &up_dir_item, 0) < 0)
        {
            printf("mkdir_cmd() not find path: %s\n", path);
            printf("mkdir_cmd() now we'll create one for you !\n");

            /**
             *  
             * 创建不存在的目录
             * 
             * dir_item -> block -> dir .
             *                   -> dir ..
             * */
            struct inode *down_inode = init_inode(TYPE_DIR);
            uint32_t inode_index = write_inode(down_inode);
            struct dir_item *dir_item = init_dir_item(TYPE_DIR, inode_index, dir_name);

            sync_inode(dir_item->inode_id, down_inode);
            /* 刷新当前节点 */
            /**
             * 刷新current_inode即可
             * current_dir_item -> current_inode -> dir_item -> down_inode -> dir .
             *                                                             -> dir ..
             */
            current_inode = read_inode(current_dir_item->inode_id);
            create_dir_item(current_inode, dir_item);
            sync_inode(current_dir_item->inode_id, current_inode);

            down_inode = mkdir_cmd(path);
            disk_write_block(0, (char *)sb);
            disk_write_block(1, (char *)sb + DEVICE_BLOCK_SIZE);
            return down_inode;
        }
    }
}
int cp_cmd(char *from_path, char *to_path)
{

    struct dir_item *from_current_dir_item;
    struct dir_item *from_last_dir_item;
    struct inode *from_current_inode;

    struct dir_item *to_current_dir_item;
    struct dir_item *to_last_dir_item;
    struct inode *to_current_inode;

    char *from_dir_name;
    char *to_dir_name;

    printf("from_path %s , to_path %s\n", from_path, to_path);

    if (search_dir_item_by_path(from_path, &from_dir_name, &from_current_dir_item, &from_last_dir_item, 0) < 0)
    {
        printf("cp_cmd() source file not exist!\n");
        return -1;
    }
    if (search_dir_item_by_path(to_path, &to_dir_name, &to_current_dir_item, &to_last_dir_item, 0) == 0)
    {
        printf("cp_cmd() dest file exists!\n");
        return -1;
    }
    from_current_inode = read_inode(from_current_dir_item->inode_id);
    to_current_inode = touch_cmd(to_path);
    if (!to_current_inode)
    {
        printf("cp_cmd() dest directory not exists!\n");
        return -1;
    }
    search_dir_item_by_path(to_path, &to_dir_name, &to_current_dir_item, &to_last_dir_item, 0);
    for (int i = 0; i < 6; i++)
    {
        uint32_t origin_block_num = from_current_inode->block_point[i];
        if (origin_block_num != 0)
        {
            uint32_t block_id = search_free_block();
            to_current_inode->block_point[i] = block_id;
            char *buf = read_block(origin_block_num);
            write_block(block_id, buf, 1024, 0);
        }
    }
    sync_inode(to_current_dir_item->inode_id, to_current_inode);
    return 0;
}

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