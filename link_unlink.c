/*********************************************
 * Programmer: Chandler Teigen
 * Date: 5/31/2020
 * Description:
 * Contains the link and unlink functions.
 * *******************************************/

#include "link_unlink.h"

int link(char *old_file, char *new_file) {
  int oino, nino, pino;
  MINODE *omip, *nmip, *pmip;
  char dir[100], base[100];

  oino = getino(old_file);
  omip = iget(dev, oino);

  if (S_ISDIR(omip->INODE.i_mode)) {
    printf("link error: %s is a directory\n", old_file);
  } else {
    nino = getino(new_file);
    if (nino == 0) {
      divide_pathname(new_file, dir, base);
      pino = getino(dir);
      pmip = iget(dev, pino);
      enter_name(pmip, oino, base);

      omip->INODE.i_links_count++;
      omip->dirty = 1;
      iput(omip);
      iput(pmip);
    } else {
      printf("link error: %s already exists\n", new_file);
    }
  }
}

int ulink(char *filename) {
  int ino, pino;
  MINODE *mip, *pmip;
  char dir[100], base[100];

  ino = getino(filename);
  mip = iget(dev, ino);
  if (S_ISDIR(mip->INODE.i_mode) || S_ISREG(mip->INODE.i_mode)) {
    divide_pathname(filename, dir, base);
    pino = getino(dir);
    pmip = iget(dev, pino);
    rm_child(pmip, base);
    pmip->dirty = 1;
    iput(pmip);
    mip->INODE.i_links_count--;

    if (mip->INODE.i_links_count > 0) {
      mip->dirty = 1;
    } else {
      // deallocate every block in the inode
      for (int i = 0; i < mip->INODE.i_blocks; i++) {
        bdalloc(dev, mip->INODE.i_block[i]);
      }
    }

    iput(mip);
  }
}
