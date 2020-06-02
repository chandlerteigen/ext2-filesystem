/*********************************************
 * Programmer: Chandler Teigen
 * Date: 6/2/2020
 * *******************************************/

#ifndef RMDIR_H
#define RMDIR_H

#include "type.h"

int rm_child(MINODE *pmip, char *name);

/************************************************
 * Function: ct_rmdir
 * Programmer: Chandler Teigen
 * Description: takes in a pathname, check if it is
 * an empty directory and is not in use, then removes 
 * it.
 * ***********************************************/
int ct_rmdir(char *pathname);

#endif