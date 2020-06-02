/*********************************************
 * Programmer: Chandler Teigen
 * Date: 6/2/2020
 * *******************************************/

#ifndef WRITE_CP_H
#define WRITE_CP_H

#include "type.h"

extern int dev;
extern PROC *running;

/************************************************
 * Function: ct_cp
 * Programmer: Chandler Teigen
 * Description: reads the contents of the source file
 * and writes them to the destination file.
 * ***********************************************/
int ct_cp(char *srcpath, char *destpath);

/************************************************
 * Function: ct_write
 * Programmer: Chandler Teigen
 * Description: writes nbytes bytes from the memory
 * pointed to by buf into the file indicated by the
 * file descriptor.
 * ***********************************************/
int ct_write(int fd, char *buf, int nbytes);

/************************************************
 * Function: write_file
 * Programmer: Chandler Teigen
 * Description: Prompts the user for a file descriptor
 * and a string to write to the file, then it calls
 * ct_write.
 * ***********************************************/
int write_file();

#endif