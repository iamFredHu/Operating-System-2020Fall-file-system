#ifndef INODE_H
#define INODE_H

#include "file_system.h"

inode *init_inode(uint16_t file_type);
inode *read_inode(uint32_t inode_id);
int sync_inode(uint32_t inode_id, struct inode *update_inode);
uint32_t write_inode(struct inode *inode);
void alloc_inode(uint32_t inode_number);
uint32_t get_free_inode();

#endif