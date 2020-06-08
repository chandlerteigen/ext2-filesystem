#ifndef SYMLINK_H
#define SYMLINK_H

#include "type.h"

// global variable defined in main.c
extern int dev;

/************************************************
 * Function: ct_symlink
 * Programmer: Chandler Teigen
 * Description: creates a symbolic link file.
 * ***********************************************/
int ct_symlink(char *old_file, char *new_file);

int ct_readlink(char *filename, char *buf, int size);

#endif