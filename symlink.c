#include "symlink.h"
#include "util.h"
#include "mkdir_creat.h"

int ct_symlink(char *old_file, char *new_file) {
  int oino, nino, pino;
  MINODE *omip, *nmip, *pmip;
  char dir[100], base[100];

  oino = getino(old_file);
  omip = iget(dev, oino);

  nino = getino(new_file);
  // file does not yet exist
  if (nino == 0) {
    divide_pathname(new_file, dir, base);
    pino = getino(dir);
    pmip = iget(dev, pino);
    ct_creat_file(new_file);
    nino = getino(new_file);
    nmip = iget(dev, nino);
    nmip->INODE.i_mode = 0120000 | omip->INODE.i_mode;
    strcpy((char *)nmip->INODE.i_block, old_file);
    nmip->INODE.i_size = strlen(old_file) + 1;
    nmip->dirty = 1;
    iput(nmip);
  } else {
    printf("link error: %s already exists\n", new_file);
  }
}

int ct_readlink(char *filename, char *buf, int size) {
  int ino;
  MINODE *mip;
  ino = getino(filename);
  mip = iget(dev, ino);
  if (S_ISLNK(mip->INODE.i_mode)) {
    strncpy(buf, (char *)mip->INODE.i_block, size);
  } else {
    printf("readlink error: %s is not a symlink\n", filename);
  }
  return strlen(buf);
}