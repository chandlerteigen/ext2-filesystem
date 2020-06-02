/*********************************************
 * Programmer: Chandler Teigen
 * Date: 6/2/2020
 * *******************************************/

#ifndef OPEN_CLOSE_LSEEK_H
#define OPEN_CLOSE_LSEEK_H

#include "type.h"


// global variables defined in main.c
extern int dev;
extern PROC *running;
extern MINODE *root;

/************************************************
 * Function: ct_truncate
 * Programmer: Chandler Teigen
 * Description: iterates over the inode's data blocks
 * and sets the bytes in each block to zero. Handles
 * indirect and double indirect blocks if they have
 * been allocated.
 * ***********************************************/

int ct_truncate(MINODE *mip);

 /************************************************
 * Function: open_file
 * Programmer: Chandler Teigen
 * Description: prompts the user for a pathname
 * and a mode Then calls ct_open.
 * ***********************************************/
int open_file();


/************************************************
 * Function: ct_open
 * Programmer: Chandler Teigen
 * Description: opens the filename indicated by the
 * pathname with the mode indicated by flags.
 * 0 1 2 3 for Read, Write, Read/Write and Append
 * ***********************************************/

int ct_open(char *pathname, int flags);

/************************************************
 * Function: close_file
 * Programmer: Chandler Teigen
 * Description: prompts the user for a file descriptor
 * then calls ct_close.
 * ***********************************************/
int close_file();

/************************************************
 * Function: ct_close
 * Programmer: Chandler Teigen
 * Description: closes the file attached to the
 * file descriptor. and releases the minode and 
 * OFT if necessary.
 * ***********************************************/

int ct_close(int fd);

/************************************************
 * Function: lseek_file
 * Programmer: Chandler Teigen
 * Description: prompts the user for a file descriptor
 * and a byte position, then calls ct_lseek with
 * those arguments.
 * ***********************************************/
int lseek_file();

/************************************************
 * Function: ct_lseek
 * Programmer: Chandler Teigen
 * Description: changes the files offset to the 
 * desired byte position.
 * ***********************************************/

int ct_lseek(int fd, int position);


/************************************************
 * Function: pfd
 * Programmer: Chandler Teigen
 * Description: prints the current open file descriptors
 * for the running process.
 * ***********************************************/

int pfd();


/************************************************
 * Function: dup
 * Programmer: Chandler Teigen
 * Description: duplicates the file descriptor into
 * the smallest free file descriptor of the running
 * process.
 * ***********************************************/

int dup(int fd);


/************************************************
 * Function: dup2
 * Programmer: Chandler Teigen
 * Description: duplicates the first file descriptor
 * into the second. If gd is open, it is closed first.
 * ***********************************************/

int dup2(int fd, int gd);

#endif