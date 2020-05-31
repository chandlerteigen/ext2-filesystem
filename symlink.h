#ifndef SYMLINK_H
#define SYMLINK_H

#include "type.h"

// global variable defined in main.c
extern int dev;

/************************************************
 * Function: symlink
 * Programmer: Chandler Teigen
 * Description: creates a symbolic link file.
 * ***********************************************/
int symlink(char *old_file, char *new_file);

int readlink(char *filename, char *buf, int size);

#endif