#ifndef INODE_H
#define INODE_H

inode* init_inode(uint16_t file_type);
uint32_t get_free_inode();
void alloc_inode(uint32_t inode_num);
void release_inode(uint32_t inode_num);
uint32_t write_inode(struct inode *inode);
int sync_inode(uint32_t inode_num, struct inode *update_inode);
struct inode * read_inode(uint32_t inode_num);

#endif