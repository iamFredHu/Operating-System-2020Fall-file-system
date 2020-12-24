#ifndef DIR_ITEM_H
#define DIR_ITEM_H

#include "init_fs.h"

dir_item *init_dir_item(uint8_t type, uint32_t inode_id, char *name);
int write_dir_item(struct inode *dir_node, struct dir_item *dir_item);
dir_item *read_dir_item(inode *dir_node, int block_point_index, int offset_index);
dir_item *search_dir_item_in_inode_by_name(inode *dir_inode, char *dir_name, int *blk_index, int *blk_off, int footprint);
int search_dir_item_by_path(char *path, char **dir_name, struct dir_item **current_dir_item, struct dir_item **up_dir_item, int is_follow);


#endif