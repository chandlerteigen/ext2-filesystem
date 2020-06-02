/*********************************************
 * Programmer: Chandler Teigen
 * Date: 5/31/2020
 * Description:
 * A Linux compatible ext2 filesystem that 
 * implements many of the common commands
 * *******************************************/

#include "type.h"
#include "util.h"
#include "symlink.h"
#include "link_unlink.h"
#include "mkdir_creat.h"
#include "open_close_lseek.h"
#include "read_cat.h"
#include "rmdir.h"

#include "cd_ls_pwd.c"
#include "write_cp.c"

// global variables
MINODE minode[NMINODE];
MINODE *root;

PROC proc[NPROC], *running;

char gpath[128]; // global for tokenized components
char *name[32];  // assume at most 32 components in pathname
int n;           // number of component strings
int permission_bits;
int fd, dev;
int nblocks, ninodes, bmap, imap, inode_start; // disk parameters
int dup_fd, dup_gd;


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

char *disk = "diskimage";
int main(int argc, char *argv[]) {
  int ino;
  char buf[BLKSIZE];
  char line[128], cmd[32], pathname[128], pathname2[128];

  printf("checking EXT2 FS ....");
  if ((fd = open(disk, O_RDWR)) < 0) {
    printf("open %s failed\n", disk);
    exit(1);
  }
  dev = fd; // fd is the global dev

  /********** read super block  ****************/
  get_block(dev, 1, buf);
  sp = (SUPER *)buf;

  /* verify it's an ext2 file system ***********/
  if (sp->s_magic != 0xEF53) {
    printf("magic = %x is not an ext2 filesystem\n", sp->s_magic);
    exit(1);
  }
  printf("EXT2 FS OK\n");
  ninodes = sp->s_inodes_count;
  nblocks = sp->s_blocks_count;

  get_block(dev, 2, buf);
  gp = (GD *)buf;

  bmap = gp->bg_block_bitmap;
  imap = gp->bg_inode_bitmap;
  inode_start = gp->bg_inode_table;
  printf("bmp=%d imap=%d inode_start = %d\n", bmap, imap, inode_start);

  init();
  mount_root();
  printf("root refCount = %d\n", root->refCount);

  printf("creating P0 as running process\n");
  running = &proc[0];
  running->status = READY;
  running->cwd = iget(dev, 2);
  printf("root refCount = %d\n", root->refCount);

  // WRTIE code here to create P1 as a USER process
  printf("\n\nInstructions for inputs: \n");
  printf("ls [pathname]\n");
  printf("cd [pathname]\n");
  printf("pwd\n");
  printf("mkdir [pathname]\n");
  printf("creat [pathname]\n");
  printf("rmdir [pathname]\n");
  printf("link [pathname] [linkname]\n");
  printf("unlink [pathname]\n");
  printf("symlink [pathname] [linkname]\n");
  printf("chmod [permission bits] [pathname]\n");
  printf("touch [pathname]\n");
  printf("quit\n");
  printf("open\n");
  printf("close\n");
  printf("lseek\n");
  printf("read\n");
  printf("write\n");
  printf("cat [pathname]\n");
  printf("pfd\n");
  printf("dup\n");
  printf("dup2\n\n");

  while (1) {
    printf("input command : "
           "[ls|cd|pwd|mkdir|creat|rmdir|link|unlink|symlink|chmod|touch|quit] "
           "\n");
    printf("                [open|close|lseek|read|write|cat|pfd|dup|dup2] : ");
    fgets(line, 128, stdin);
    line[strlen(line) - 1] = 0;

    if (line[0] == 0)
      continue;
    pathname[0] = 0;

    sscanf(line, "%s %s", cmd, pathname);
    printf("cmd=%s pathname=%s\n", cmd, pathname);

    if (strcmp(cmd, "ls") == 0)
      ls(pathname);
    else if (strcmp(cmd, "cd") == 0)
      mychdir(pathname);
    else if (strcmp(cmd, "pwd") == 0)
      pwd(running->cwd);
    else if (strcmp(cmd, "quit") == 0)
      quit();
    else if (strcmp(cmd, "mkdir") == 0)
      ct_mkdir(pathname);
    else if (strcmp(cmd, "creat") == 0)
      ct_creat_file(pathname);
    else if (strcmp(cmd, "rmdir") == 0)
      ct_rmdir(pathname);
    else if (strcmp(cmd, "link") == 0) {
      sscanf(line, "%s %s %s", cmd, pathname, pathname2);
      link(pathname, pathname2);
    } else if (strcmp(cmd, "unlink") == 0)
      ulink(pathname);
    else if (strcmp(cmd, "symlink") == 0) {
      sscanf(line, "%s %s %s", cmd, pathname, pathname2);
      symlink(pathname, pathname2);
    } else if (strcmp(cmd, "chmod") == 0) {
      sscanf(line, "%s %o %s", cmd, &permission_bits, pathname);
      ct_chmod(pathname, permission_bits);
    } else if (strcmp(cmd, "touch") == 0)
      ct_touch(pathname);
    else if (strcmp(cmd, "cat") == 0)
      cat(pathname);
    else if (strcmp(cmd, "cp") == 0) {
      sscanf(line, "%s %s %s", cmd, pathname, pathname2);
      ct_cp(pathname, pathname2);
    } else if (strcmp(cmd, "pfd") == 0) {
      pfd();
    } else if (strcmp(cmd, "dup") == 0) {
      sscanf(line, "%s %d", cmd, &dup_fd);
      dup(dup_fd);
    } else if (strcmp(cmd, "dup2") == 0) {
      sscanf(line, "%s %d %d", cmd, &dup_fd, &dup_gd);
      dup2(dup_fd, dup_gd);
    } else if (strcmp(cmd, "open") == 0) {
      open_file();
    } else if (strcmp(cmd, "close") == 0) {
      close_file();
    } else if(strcmp(cmd, "write") == 0) {
      write_file();
    } else if(strcmp(cmd, "read") == 0) {
      read_file();
    } else if(strcmp(cmd, "lseek") == 0) {
      lseek_file();
    }
  }
}

int quit() {
  int i;
  MINODE *mip;
  for (i = 0; i < NMINODE; i++) {
    mip = &minode[i];
    if (mip->refCount > 0)
      iput(mip);
  }
  exit(0);
}
