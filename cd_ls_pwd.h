/*********************************************
 * Programmer: Chandler Teigen
 * Date: 6/2/2020
 * *******************************************/

#ifndef CD_LS_PWD_H
#define CD_LS_PWD_H

#include "type.h"

// global variables defined in main.c
extern int dev;
extern PROC *running;
extern MINODE *root;

/************************************************
 * Function: int mychdir(char *pathname)
 * Programmer: Chandler Teigen
 * Description:#include "main.c"irectory to pathname.
 * adapted from algorithm on (Wang, 330)
 * ***********************************************/
int mychdir(char *pathname);

/************************************************
 * Function: int ls_file(MINODE *mip, char *name)
 * Programmer: Chandler Teigen
 * Description:
 * provides ls info for a single file. adapted from algorithm
 * provided in (Wang, 330)
 * ***********************************************/
int ls_file(MINODE *mip, char *name);

/************************************************
 * Function: int ls(char *pathname)
 * Programmer: Chandler Teigen
 * Description:
 * implements the ls command for ext2 fs.
 * ***********************************************/
int ls(char *pathname);

/************************************************
 * Function: char *pwd(MINOODE *wd)
 * Programmer: Chandler Teigen
 * Description:
 * implements the recursive part of the pwd algorithm
 * in (Wang, 330)
 * ***********************************************/
char *rpwd(MINODE *wd);
/************************************************
 * Function: char *pwd(MINODE *wd)
 * Programmer: Chandler Teigen
 * Description:
 * either prints root symbol, or calls recursive pwd.
 * ***********************************************/
char *pwd(MINODE *wd);

#endif