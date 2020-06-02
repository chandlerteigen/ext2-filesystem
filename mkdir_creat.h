/*********************************************
 * Programmer: Chandler Teigen
 * Date: 6/2/2020
 * *******************************************/

#ifndef MKDIR_CREAT_H
#define MKDIR_CREAT_H

#include "type.h"

/************************************************
 * Function: int enter_name(MINODE *pip, int ino, char *name)
 * Programmer: Chandler Teigen
 * Description:
 * adds the directory name to the parent inode.
 * Follows algorithm from (Wang, 334)
 * ***********************************************/
int enter_name(MINODE *pip, int ino, char *name);

/************************************************
 * Function: int divide_pathname(char *pathname, char *dir, char *base)
 * Programmer: Chandler Teigen
 * Description:
 * splits the pathname string into a dir, and a base file name.
 * ***********************************************/
int divide_pathname(char *pathname, char *dir, char *base);

/************************************************
 * Function: int ct_mkdir(char *pathname)
 * Programmer: Chandler Teigen
 * Description:
 * implements mkdir by using the algorithm described by
 * (Wang, 332)
 * ***********************************************/
int ct_mkdir(char *pathname);

/************************************************
 * Function: ct_mkdir_helper(MINODE *pmip, char *base)
 * Programmer: Chandler Teigen
 * Description:
 * helper function continuing the algorith from (Wang, 332)
 * ***********************************************/
int ct_mkdir_helper(MINODE *pmip, char *base);

/************************************************
 * Function: int ct_creat_file(char *pathname)
 * Programmer: Chandler Teigen
 * Description:
 * essntially a copy and paste from ct_mkdir, but
 * with different file type bits.
 * ***********************************************/
int ct_creat_file(char *pathname);

/************************************************
 * Function: int ct_creat_file_helper(MINODE *pmip, char *base)
 * Programmer: Chandler Teigen
 * Description:
 * another copy paste of the mkdir code above
 * ***********************************************/
int ct_creat_file_helper(MINODE *pmip, char *base);

#endif