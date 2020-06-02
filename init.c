/*********************************************
 * Programmer: Chandler Teigen
 * Date: 6/2/2020
 * Description:
 * Contains the init and mount_root functions for
 * preparing the filesystem for use.
 * *******************************************/

#include "init.h"
#include "util.h"

int init() {
  int i, j;
  MINODE *mip;
  PROC *p;

  printf("init()\n");

  for (i = 0; i < NMINODE; i++) {
    mip = &minode[i];
    mip->dev = mip->ino = 0;
    mip->refCount = 0;
    mip->mounted = 0;
    mip->mptr = 0;
  }
  for (i = 0; i < NPROC; i++) {
    p = &proc[i];
    p->pid = i;
    p->uid = p->gid = 0;
    p->cwd = 0;
    p->status = FREE;
    for (j = 0; j < NFD; j++)
      p->fd[j] = 0;
  }
}

// load root INODE and set root pointer to it
int mount_root() {
  printf("mount_root()\n");
  root = iget(dev, 2);
}