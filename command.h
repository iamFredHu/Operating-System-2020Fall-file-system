#ifndef COMMAND_H
#define COMMAND_H

#include "file_system.h"

void shutdown_cmd();
void ls_cmd(char *path);
inode *touch_cmd(char *path);
inode *mkdir_cmd(char *path);
int cp_cmd(char *from_path, char *to_path);


#endif