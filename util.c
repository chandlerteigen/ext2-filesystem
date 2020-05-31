/*********************************************
 * Programmer: Chandler Teigen
 * Date: 5/31/2020
 * Description:
 * Contains the misc. utility functions used
 * throughout the filesystem code.
 * *******************************************/

#include "util.h"

// Unless otherwise stated, all following functions are provided by KC Wang in
// the ext2 FS mount root starter code. CS 360 Spring 2020

int get_block(int dev, int blk, char *buf) {
  lseek(dev, (long)blk * BLKSIZE, 0);
  read(dev, buf, BLKSIZE);
}
int put_block(int dev, int blk, char *buf) {
  lseek(dev, (long)blk * BLKSIZE, 0);
  write(dev, buf, BLKSIZE);
}

int tokenize(char *pathname) {
  int i;
  char *s;
  // printf("tokenize %s\n", pathname);

  strcpy(gpath, pathname); // tokens are in global gpath[ ]
  n = 0;

  s = strtok(gpath, "/");
  while (s) {
    name[n] = s;
    n++;
    s = strtok(0, "/");
  }
}

// return minode pointer to loaded INODE
MINODE *iget(int dev, int ino) {
  int i;
  MINODE *mip;
  char buf[BLKSIZE];
  int blk, offset;
  INODE *ip;

  for (i = 0; i < NMINODE; i++) {
    mip = &minode[i];
    if (mip->dev == dev && mip->ino == ino) {
      mip->refCount++;
      // printf("found [%d %d] as minode[%d] in core\n", dev, ino, i);
      return mip;
    }
  }

  for (i = 0; i < NMINODE; i++) {
    mip = &minode[i];
    if (mip->refCount == 0) {
      // printf("allocating NEW minode[%d] for [%d %d]\n", i, dev, ino);
      mip->refCount = 1;
      mip->dev = dev;
      mip->ino = ino;

      // get INODE of ino into buf[ ]
      blk = (ino - 1) / 8 + inode_start;
      offset = (ino - 1) % 8;
      /* code */
      get_block(dev, blk, buf);
      ip = (INODE *)buf + offset;
      // copy INODE to mp->INODE
      mip->INODE = *ip;
      return mip;
    }
  }
  printf("PANIC: no more free minodes\n");
  return 0;
}

void iput(MINODE *mip) {
  int i, block, offset;
  char buf[BLKSIZE];
  INODE *ip;

  mip->refCount--;

  if (mip->refCount > 0) // minode is still in use
    return;
  if (!mip->dirty) // INODE has not changed; no need to write back
    return;

  // adapted from (Wang, 324)
  block = (mip->ino - 1) / 8 + inode_start;
  offset = (mip->ino - 1) % 8;
  get_block(dev, block, buf);
  ip = (INODE *)buf + offset;
  *ip = mip->INODE;
  put_block(dev, block, buf);
  mip->refCount = 0;
}

int search(MINODE *mip, char *name) {
  char *cp, c, sbuf[BLKSIZE], temp[256];
  DIR *dp;
  INODE *ip;

  ip = &(mip->INODE);

  /*** search for name in mip's data blocks: ASSUME i_block[0] ONLY ***/

  get_block(dev, ip->i_block[0], sbuf);
  dp = (DIR *)sbuf;
  cp = sbuf;
  // printf("  ino   rlen  nlen  name\n");

  while (cp < sbuf + BLKSIZE) {
    strncpy(temp, dp->name, dp->name_len);
    temp[dp->name_len] = 0;

    if (strcmp(temp, name) == 0) {
      return dp->inode;
    }
    cp += dp->rec_len;
    dp = (DIR *)cp;
  }
  return 0;
}

int getino(char *pathname) {
  int i, ino, blk, disp;
  char buf[BLKSIZE];
  INODE *ip;
  MINODE *mip;

  if (strcmp(pathname, "/") == 0)
    return 2;

  // starting mip = root OR CWD
  if (pathname[0] == '/')
    mip = root;
  else
    mip = running->cwd;

  mip->refCount++; // because we iput(mip) later

  tokenize(pathname);

  for (i = 0; i < n; i++) {
    ino = search(mip, name[i]);

    if (ino == 0) {
      iput(mip);
      // printf("name %s does not exist\n", name[i]);
      return 0;
    }
    iput(mip);            // release current mip
    mip = iget(dev, ino); // get next mip
  }

  iput(mip); // release mip
  return ino;
}

int findino(MINODE *mip, u32 *myino) // myino = ino of . return ino of ..
{
  char buf[BLKSIZE], *cp;
  DIR *dp;
  int ino = 0;
  get_block(mip->dev, mip->INODE.i_block[0], buf);
  cp = buf;
  dp = (DIR *)cp;

  while (cp < buf + BLKSIZE) {
    if (strcmp(dp->name, ".") == 0) {
      *myino = dp->inode;
    } else if (strcmp(dp->name, "..") == 0) {
      ino = dp->inode;
    }
    cp += dp->rec_len;
    dp = (DIR *)cp;
  }
  return ino;
}

int get_myino(MINODE *mip, int *parent_ino) {

  char buf[BLKSIZE];
  char *cp;
  int my_ino;

  get_block(dev, mip->INODE.i_block[0], buf);
  dp = (DIR *)buf;
  cp = buf;

  my_ino = dp->inode;

  cp += dp->rec_len;
  dp = (DIR *)cp;
  *parent_ino = dp->inode;

  return my_ino;
}

int get_myname(MINODE *parent_minode, int my_ino, char *my_name) {
  char buf[BLKSIZE];
  char *cp;
  DIR *dp;

  get_block(dev, parent_minode->INODE.i_block[0], buf);
  dp = (DIR *)buf;
  cp = buf;
  while (dp->inode != my_ino) {
    cp += dp->rec_len;
    dp = (DIR *)cp;
  }
  strncpy(my_name, dp->name, dp->name_len);
  my_name[dp->name_len] = 0;
}

int tst_bit(char *buf, int bit) {
  int i, j, status;

  i = bit / 8;
  j = bit % 8;

  if (buf[i] & (1 << j)) {
    status = 1;
  } else {
    status = 0;
  }
  return status;
}

int set_bit(char *buf, int bit) {
  int i, j;

  i = bit / 8;
  j = bit % 8;
  buf[i] |= (1 << j);
}

int clr_bit(char *buf, int bit) {
  // find the byte that the bit is located in
  // then found its location in the byte
  // then AND it with the not of its current value
  // this will always clear the bit
  buf[bit / 8] &= ~(1 << (bit % 8));
}

int ialloc(int dev) // allocate an inode number from inode_bitmap
{
  int i;
  char buf[BLKSIZE];

  get_block(dev, imap, buf);

  for (i = 0; i < ninodes; i++) {
    if (tst_bit(buf, i) == 0) {
      set_bit(buf, i);
      put_block(dev, imap, buf);
      printf("allocated ino = %d\n", i + 1); // bits count from 0; ino from 1
      dec_free_inodes(dev); // decrement the number of free inodes
      return i + 1;         // return inode number
    }
  }
  return 0;
}

int idalloc(int dev, int ino) {
  char buf[BLKSIZE];

  // read inode_bitmap_block
  get_block(dev, imap, buf);
  // put the changed block back into memory
  put_block(dev, imap, buf);

  // increment the free counters to reflect the changes to the imap
  inc_free_inodes(dev);

  printf("Deallocated inode %d\n", ino);

  return 0;
}

int balloc(int dev) {
  int i;
  char buf[BLKSIZE];

  get_block(dev, bmap, buf);
  // memset(buf, 0, BLKSIZE); // zero out the block
  for (i = 0; i < nblocks; i++) {
    if (tst_bit(buf, i) == 0) // indicates that the blk is FREE
    {
      set_bit(buf, i); // set to IN USE
      put_block(dev, bmap, buf);
      printf("allocated blk = %d\n", i + 1);
      return i + 1; // return the blk number
    }
  }
  return 0; // indicates failure to allocate new blk
}

int bdalloc(int dev, int bno) {
  char buf[BLKSIZE];

  // get the block bitmap and store it in buf
  get_block(dev, bmap, buf);

  // clear the bit associated with bno#include "main.c"
  clr_bit(buf, bno - 1);

  put_block(dev, bmap, buf);

  printf("Deallocated block %d\n", bno);
}

int inc_free_inodes(int dev) {
  SUPER *sp;
  GD *gd;
  char buf[BLKSIZE];

  // get super block
  get_block(dev, 1, buf);
  sp = (SUPER *)buf;

  sp->s_free_inodes_count++;
  put_block(dev, 1, buf);
  get_block(dev, 2, buf);

  // get group descriptor
  gd = (GD *)buf;
  gd->bg_free_inodes_count++;
  put_block(dev, 2, buf);
}

int dec_free_inodes(int dev) {
  SUPER *sp;
  GD *gd;
  char buf[BLKSIZE];

  // get super block
  get_block(dev, 1, buf);
  sp = (SUPER *)buf;

  sp->s_free_inodes_count--;
  put_block(dev, 1, buf);
  get_block(dev, 2, buf);

  // get group descriptor
  gd = (GD *)buf;
  gd->bg_free_inodes_count--;
  put_block(dev, 2, buf);
}

int ct_chmod(char *filename, int octal_permission) {
  int ino;
  MINODE *mip;

  ino = getino(filename);
  if (getino != 0) {
    mip = iget(dev, ino);
    mip->INODE.i_mode = octal_permission;
    mip->dirty = 1;
    iput(mip);
  } else {
    printf("chmod error: %s does not exist\n", filename);
  }
}

int ct_touch(char *filename) {
  int ino;
  MINODE *mip;
  ino = getino(filename);
  if (ino != 0) {
    mip = iget(dev, ino);
    mip->INODE.i_atime = mip->INODE.i_ctime = mip->INODE.i_mtime = time(0L);
    mip->dirty = 1;
    iput(mip);
  } else {
    printf("touch: %s did not exist, creating new file\n", filename);
    ct_creat_file(filename);
  }
}

int ct_stat(char *filename, struct stat *buf) {
  int ino;
  MINODE *mip;

  ino = getino(filename);

  if (ino != 0) {
    mip = iget(dev, ino);
    buf->st_dev = mip->dev;
    buf->st_ino = mip->ino;
    buf->st_atime = mip->INODE.i_atime;
    buf->st_blocks = mip->INODE.i_blocks;
    buf->st_ctime = mip->INODE.i_ctime;
    buf->st_gid = mip->INODE.i_gid;
    buf->st_mode = mip->INODE.i_mode;
    buf->st_mtime = mip->INODE.i_mtime;
    buf->st_nlink = mip->INODE.i_links_count;
    buf->st_size = mip->INODE.i_size;
    buf->st_uid = mip->INODE.i_uid;
    iput(mip);
  } else {
    printf("stat error: %s does not exist\n", filename);
  }
}