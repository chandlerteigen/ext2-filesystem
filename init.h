/*********************************************
 * Programmer: Chandler Teigen
 * Date: 6/2/2020
 * *******************************************/

#ifndef INIT_H
#define INIT_H

#include "type.h"

// global variables defined in main.c
extern PROC proc[NPROC];
extern MINODE minode[NMINODE];
extern MINODE *root;
extern int dev;

int init();

int mount_root();

#endif