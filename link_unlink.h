/*********************************************
 * Programmer: Chandler Teigen
 * Date: 6/2/2020
 * *******************************************/

#ifndef LINK_UNLINK_H
#define LINK_UNLINK_H

#include "type.h"

/************************************************
 * Function: link
 * Programmer: Chandler Teigen
 * Description: creates a link file that is linked
 * to the old_file.
 * ***********************************************/
int link(char *old_file, char *new_file);

/************************************************
 * Function: ulink
 * Programmer: Chandler Teigen
 * Description: unlinks the file indicated by
 * the filename
 * ***********************************************/
int ulink(char *filename);

#endif