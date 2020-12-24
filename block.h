#ifndef BLOCK_H
#define BLOCK_H

uint32_t search_free_block();
void alloc_block(uint32_t block_id);
char *read_block(uint32_t block_id);
int write_block(uint32_t block_id, char *buf, int size, int offset);


#endif