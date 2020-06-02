/*********************************************
 * Programmer: Chandler Teigen
 * Date: 6/2/2020
 * *******************************************/

#ifndef READ_CAT_H
#define READ_CAT_H

#include "type.h"

// global variables defined in main.c
extern PROC *running;
extern int dev;

/************************************************
 * Function: find_min
 * Programmer: Chandler Teigen
 * Description: returns the minimum of the three
 * integer arguments
 * ***********************************************/

int find_min(int num1, int num2, int num3);

/************************************************
 * Function: cat
 * Programmer: Chandler Teigen
 * Description: opens the file for read, prints the
 * contents to the console, then closes the file.
 * ***********************************************/
int cat(char *pathname);

/************************************************
 * Function: ct_read
 * Programmer: Chandler Teigen
 * Description: reads nbytes bytes from the file
 * descriptor into buf.
 * ***********************************************/
int ct_read(int fd, char *buf, int nbytes);

/************************************************
 * Function: read_file
 * Programmer: Chandler Teigen
 * Description: prompts the user for a file descriptor
 * and a number of bytes,
 * ***********************************************/
int read_file();

#endif