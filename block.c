#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "command.h"
#include "disk.h"
#include "inode.h"
#include "init_fs.h"
#include "utils.h"
#include "dir_item.h"
#include "block.h"
#include "path.h"

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
