/*********************************************
 * Programmer: Chandler Teigen
 * Date: 5/31/2020
 * Description:
 * Contains the functions that do that commands
 * mkdir and creat.
 * *******************************************/

#include "mkdir_creat.h"
#include "util.h"

int enter_name(MINODE *pip, int ino, char *name) {
  int i, remaining, needed_length, dp_ideal, current_block;
  char buf[BLKSIZE];
  DIR *dp, *old_last_dp;
  char *cp;
  for (i = 0; i < 12; i++) {
    if (pip->INODE.i_block[i] != 0) {
      current_block = pip->INODE.i_block[i];
      get_block(pip->dev, current_block, buf);
      dp = (DIR *)buf;
      cp = buf;
      while (cp + dp->rec_len < buf + BLKSIZE) {
        cp += dp->rec_len;
        dp = (DIR *)cp;
      }

      // now dp points at the last entry in the block
      old_last_dp = dp;
      dp_ideal = 4 * ((8 + dp->name_len + 3) / 4);
      remaining = dp->rec_len - dp_ideal;
      needed_length = 4 * ((8 + strlen(name) + 3) / 4);
      if (remaining >= needed_length) {
        dp = (DIR *)((char *)dp + dp_ideal);
        dp->inode = ino;
        dp->rec_len =
            old_last_dp->rec_len -
            dp_ideal; // old dp rec_len minus the new ideal length of dp
        dp->name_len = strlen(name);
        strcpy(dp->name, name);
        // change the rec_len of dp to its ideal length
        old_last_dp->rec_len = dp_ideal;
      } else {
        current_block = balloc(dev); // try to allocate new block
        if (current_block) {
          pip->INODE.i_block[i] = current_block;
          pip->INODE.i_size += BLKSIZE;
          pip->INODE.i_blocks += 2;

          dp = (DIR *)buf;
          dp->inode = ino;
          dp->name_len = strlen(name);
          strcpy(dp->name, name);
          dp->rec_len = BLKSIZE; // will fill the entire block initially

          pip->dirty = 1; // set the parent to dirty
        } else {
          printf("mkdir error: not enough space on dev for new block");
        }
      }
      put_block(pip->dev, current_block, buf);
      break;
    }
  }
  return 0;
}

int ct_mkdir(char *pathname) {
  MINODE *start, *pmip;
  char dir[100];
  char base[100];
  int pino;

  divide_pathname(pathname, dir, base);

  pino = getino(dir);
  pmip = iget(dev, pino);
  if (S_ISDIR(pmip->INODE.i_mode) == 0) {
    printf("mkdir error: %s is not a dir\n", dir);
  } else {
    if (search(pmip, base)) {
      printf("mkdir error: %s already exists in %s\n", base, dir);
    } else {
      ct_mkdir_helper(pmip, base);
      pmip->INODE.i_links_count++;
      pmip->INODE.i_atime = time(0L);
      pmip->dirty = 1;
    }
  }
  iput(pmip);
}

int ct_mkdir_helper(MINODE *pmip, char *base) {
  int ino, blk;
  MINODE *mip;
  INODE *ip;
  ino = ialloc(dev);
  blk = balloc(dev);

  mip = iget(dev, ino);

  ip = &mip->INODE;
  ip->i_mode = 0x41ED;
  ip->i_uid = running->uid;
  ip->i_gid = running->gid;
  ip->i_size = BLKSIZE;
  ip->i_links_count = 2;
  ip->i_atime = ip->i_ctime = ip->i_mtime = time(0L);
  ip->i_blocks = 2;
  ip->i_block[0] = blk;
  for (int i = 1; i < 15; i++) {
    ip->i_block[i] = 0;
  }
  mip->dirty = 1;
  iput(mip);

  // create data block for new dir containing . and ..
  char buf[BLKSIZE];
  bzero(buf, BLKSIZE);
  DIR *dp = (DIR *)buf;
  // make .
  dp->inode = ino;
  dp->rec_len = 12;
  dp->name_len = 1;
  dp->name[0] = '.';
  // make ..
  dp = (DIR *)((char *)dp + 12);
  dp->inode = pmip->ino;
  dp->rec_len = BLKSIZE - 12;
  dp->name_len = 2;
  dp->name[0] = dp->name[1] = '.';
  put_block(dev, blk, buf);

  enter_name(pmip, ino, base);
}

int ct_creat_file(char *pathname) {
  MINODE *start, *pmip;
  char dir[100];
  char base[100];
  int pino;
  char pathname_copy[100];
  // make a copy of pathname, so it is not changed outside
  // the function call.
  strcpy(pathname_copy, pathname);
  printf("pathname %s  copy %s \n", pathname, pathname_copy);
  divide_pathname(pathname_copy, dir, base);

  pino = getino(dir);
  pmip = iget(dev, pino);
  if (S_ISDIR(pmip->INODE.i_mode) == 0) {
    printf("mkdir error: %s is not a dir\n", dir);
  } else {
    if (search(pmip, base)) {
      printf("mkdir error: %s already exists in %s\n", base, dir);
    } else {
      ct_creat_file_helper(pmip, base);
      pmip->INODE.i_atime = time(0L);
      pmip->dirty = 1;
    }
  }
  iput(pmip);
}

int ct_creat_file_helper(MINODE *pmip, char *base) {
  int ino, blk;
  MINODE *mip;
  INODE *ip;
  ino = ialloc(dev);
  // blk = balloc(dev);

  mip = iget(dev, ino);

  ip = &mip->INODE;
  ip->i_mode = 0x81A4;
  ip->i_uid = running->uid;
  ip->i_gid = running->gid;
  ip->i_size = 0;
  ip->i_links_count = 1;
  ip->i_atime = ip->i_ctime = ip->i_mtime = time(0L);
  ip->i_blocks = 2;
  // ip->i_block[0] = blk;
  for (int i = 1; i < 15; i++) {
    ip->i_block[i] = 0;
  }
  mip->dirty = 1;
  iput(mip);

  enter_name(pmip, ino, base);
}
