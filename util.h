/*********************************************
 * Programmer: Chandler Teigen
 * Date: 5/31/2020
 * *******************************************/

#ifndef UTIL_H
#define UTIL_H

#include "type.h"

// globals defined in main.c
extern char gpath[128];
extern int n;
extern char *name[32];
extern MINODE minode[NMINODE];
extern int inode_start;
extern int dev;
extern MINODE *root;
extern PROC proc[NPROC];
extern PROC *running;
extern int imap;
extern int bmap;
extern int ninodes;
extern int nblocks;

// Unless otherwise stated, all following functions are provided by KC Wang in
// the ext2 FS mount root starter code. CS 360 Spring 2020

int get_block(int dev, int blk, char *buf);

int put_block(int dev, int blk, char *buf);

int tokenize(char *pathname);

// return minode pointer to loaded INODE
MINODE *iget(int dev, int ino);

void iput(MINODE *mip);

int search(MINODE *mip, char *name);

int getino(char *pathname);

int findmyname(MINODE *parent, u32 myino, char *myname);

int findino(MINODE *mip, u32 *myino);

int get_myname(MINODE *parent_minode, int my_ino, char *my_name);

/**********************CODE ADDED FOR KC WANG ASSIGNMENT*************************************************************/

/************************************************
 * Function: int get_myino(MINODE *mip, int *parent_ino)
 * Programmer: Chandler Teigen
 * Description:
 * Return value is mip's ino, and parent ino in int *parent_ino
 * ***********************************************/
int get_myino(MINODE *mip, int *parent_ino);

/************************************************
 * Function: int tst_bit(char *buf, int bit)
 * Programmer: Chandler Teigen
 * Description:
 * Function written using equations from (Wang, 306)
 * ***********************************************/
int tst_bit(char *buf, int bit);

/************************************************
 * Function: int set_bit(char *buf, int bit)
 * Programmer: Chandler Teigen
 * Description:
 * Function written using equations from (Wang, 306)
 * ***********************************************/
int set_bit(char *buf, int bit);

/************************************************
 * Function: int clr_bit(char *buf, int bit)
 * Programmer: Chandler Teigen
 * Description:
 * Function from (Wang, 338)
 * ***********************************************/
int clr_bit(char *buf, int bit);

int ialloc(int dev);

/************************************************
 * Function: int idalloc(int dev, int ino)
 * Programmer: Chandler Teigen
 * Description:
 * deallocation an inode from the inode bitmap
 * ***********************************************/
int idalloc(int dev, int ino);
/************************************************************************************************/

/************************************************
 * Function: int balloc(int dev)
 * Programmer: Chandler Teigen
 * Description:
 * Allocates a block on the disk device.
 * adapted from ialloc code (Wang, 333)
 * ***********************************************/
int balloc(int dev);

/************************************************
 * Function: int bdalloc(int dev, int bno)
 * Programmer: Chandler Teigen
 * Description:
 * deallocates a block on the disk device.
 * ***********************************************/
int bdalloc(int dev, int bno);

/************************************************
 * Function: int inc_free_inodes(int dev)
 * Programmer: Chandler Teigen
 * Description:
 * increments the free inodes count in the superblock
 * and the group descriptor by 1.
 * implementation credit to KC Wang,(Wang, 338)
 * ***********************************************/
int inc_free_inodes(int dev);

/************************************************
 * Function: int dec_free_inodes(int dev)
 * Programmer: Chandler Teigen
 * Description:
 * decrements the free inodes count in the superblock
 * and the group descriptor by 1.
 * implementation credit to KC Wang,(Wang, 338)
 * ***********************************************/
int dec_free_inodes(int dev);

/************************************************
 * Function: int divide_pathname(char *pathname, char *dir, char *base)
 * Programmer: Chandler Teigen
 * Description:
 * splits the pathname string into a dir, and a base file name.
 * ***********************************************/
int divide_pathname(char *pathname, char *dir, char *base);

int ct_chmod(char *filename, int octal_permission);

int ct_touch(char *filename);

int ct_stat(char *filename, struct stat *buf);

#endif