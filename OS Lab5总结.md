# OS Lab5总结

## 总体结构

![placement](https://hitsz-lab.gitee.io/os_lab/lab5/part2.assets/placement.svg)

模拟磁盘的大小为4MB，每个磁盘块大小为512KB

超级块占用656B，2个磁盘块

inode占用32B dir_item占用128B 数据块大小为1KB

## block.c

本模块主要功能是处理数据块，因为原disk.c文件中提供的对于磁盘块的处理都是以buf形式，且大小为512B，所以需要写一些函数进行封装。

* uint32_t search_free_block()

找到空闲的数据块。数据块是否被占用是由位图决定的，因此只需要在block_map[]中进行按位与，如果结果为0则说明空闲，返回block_map[]的id是i*32+j（uint32，每1bit表示一个数据块，j是uint32内的位移）

* char *read_block(uint32_t block_id)

函数输入是block_id，即需要根据block_id（数据块id）找到对应的数据块，这个比较简单，设置一个buf，调用disk.c提供的接口就行，不过需要注意的是id要乘2，因为数据块大小为1KB，而磁盘块大小为512B。

* int write_block(uint32_t block_id, char *buf, int size, int offset)

写数据块需要提供block_id(数据块id)，首先需要分配一个数据块，三件事

1）更新block_map

sb->block_map[block_id / 32] = sb->block_map[block_id / 32] | (1 << block_id % 32)

2）free_block_count -1

3）disk_write_block

然后就是把数据块写到磁盘中了，

## dir_item.c

本模块主要是处理与目录项结构体相关的函数

* dir_item *init_dir_item(uint8_t type, uint32_t inode_id, char *name)

给定需要初始化的dir_item参数，包括type、inode_id、dit_name等，然后返回这个dir_item的内存地址待用

* int write_dir_item(struct inode *dir_node, struct dir_item *dir_item)

根据传入的dir_item,可以得到size（文件大小），可以计算出block_point对应的id，数据块大小为1024B，如果大小为1025B则id为1，块内偏移为1，以此类推……

如果block_point对应的指针指向0，说明应该分配一个新的数据块

如果有指向id，则修改block_id为该id 然后把信息写入磁盘中，同时修改dir_node的size，即文件尺寸

* dir_item *read_dir_item(inode *dir_node, int block_point_index, int offset_index)

根据inode读dir_item，这个是调用前面写的read_data_block，读数据块函数，需要提供block_point_index和位移量

* dir_item *search_dir_item_in_inode_by_name(inode *dir_inode_be_searched, char *dir_name, int *block_id, int *block_offset, int list_flag)

根据dir_name和inode找dir_item

查找逻辑：在6个block_point中进行查找，同时设置一个flag判断是不是最后一项，dir_item_per_block每一块的dir_item数量。

然后调用read_dir_item函数，根据inode i，j找到所有的dir_item,需要valid=1才列出来，如果dir_item->name, dir_name相等就算找到，可以返回对应的dir_item。如果已经是最后一项了，就记录下这一项的block_id

* int search_dir_item_by_path(char *path, char **dir_name, struct dir_item **current_dir_item, struct dir_item **up_dir_item)

根据path找到dir_item，实际上是通过递归的形式

比如路径a/b/c/d

有一个下级和上一级目录

a连接着b b连接着c c连接着d d后面没有连接着的东西，（后面没有/，用函数判断，说明是最后一个）

## init_fs.c

本模块对文件系统进行初始化

open_disk()，载入数据

读sp_block，如果幻数不符合，设置一个循环询问用户是否进行格式化

格式化内容：初始化超级块，设置根目录“/”

标记数据块占用情况，即为sp_block和inode分配数据块0,1-32

//初始化完毕或已有数据，社会根目录inode和dir_item

然后进入根目录

## inode.c

本模块处理inode

* inode *init_inode(uint16_t file_type)

inode初始化，给定inode参数，返回一个inode

* uint32_t get_free_inode()

找空闲inode，利用位图

* void alloc_inode(uint32_t inode_number)

分配inode，根据inode_id，算出数据块id和offset，写入磁盘块

* uint32_t write_inode(struct inode *inode)

将inode写入磁盘

* int update_inode(uint32_t inode_id, struct inode *update_inode)

修改inode信息，注意数据块+1（超级块）

* inode *read_inode(uint32_t inode_id)

根据inode_id找inode

inode_id -> block_id ->read_block 强转加偏移



## path的管理

拆分

/a/b/c/d

以/为符号