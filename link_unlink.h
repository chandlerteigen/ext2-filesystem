/*********************************************
 * Programmer: Chandler Teigen
 * Date: 6/2/2020
 * *******************************************/

#ifndef LINK_UNLINK_H
#define LINK_UNLINK_H

#include "type.h"

// global variable declared in main.c
extern int dev;

/************************************************
 * Function: ct_link
 * Programmer: Chandler Teigen
 * Description: creates a link file that is linked
 * to the old_file.
 * ***********************************************/
int ct_link(char *old_file, char *new_file);

/************************************************
 * Function: ct_unlink
 * Programmer: Chandler Teigen
 * Description: unlinks the file indicated by
 * the filename
 * ***********************************************/
int ct_unlink(char *filename);

#endif