/*********************************************
 * Programmer: Chandler Teigen
 * Date: 5/31/2020
 * Description:
 * Contains the functions for the cd, ls and pwd
 * commands.
 * *******************************************/

#include "cd_ls_pwd.h"

/************************************************
 * Function: int mychdir(char *pathname)
 * Programmer: Chandler Teigen
 * Description:#include "main.c"irectory to pathname.
 * adapted from algorithm on (Wang, 330)
 * ***********************************************/
int mychdir(char *pathname) {
  printf("chdir %s\n", pathname);
  int ino = getino(pathname);
  MINODE *mip;
  if (ino == 0) {
    printf("chdir error: ino = 0\n");
  } else {
    mip = iget(dev, ino);
    if (S_ISDIR(mip->INODE.i_mode)) {
      printf("changing dir\n");
      iput(running->cwd);
      running->cwd = mip;
    } else {
      printf("chdir error: %s is not a directory\n", pathname);
    }
  }
}

/************************************************
 * Function: int ls_file(MINODE *mip, char *name)
 * Programmer: Chandler Teigen
 * Description:
 * provides ls info for a single file. adapted from algorithm
 * provided in (Wang, 330)
 * ***********************************************/
int ls_file(MINODE *mip, char *name) {
  char *t1 = "xwrxwrxwr-------";
  char ftime[64];
  char linkbuf[256];

  if (S_ISREG(mip->INODE.i_mode)) {
    printf("%c", '-');
  }
  if (S_ISDIR(mip->INODE.i_mode)) {
    printf("%c", 'd');
  }
  if (S_ISLNK(mip->INODE.i_mode)) {
    printf("%c", 'l');
  }
  for (int i = 8; i >= 0; i--) {
    if (mip->INODE.i_mode & (1 << i)) {
      printf("%c", t1[i]);
    } else {
      printf("-");
    }
  }

  printf("%4d ", mip->INODE.i_links_count);
  printf("%4d ", mip->INODE.i_gid);
  printf("%4d ", mip->INODE.i_uid);
  printf("%4d   ", mip->INODE.i_size);
  time_t atime = mip->INODE.i_atime;
  strcpy(ftime, ctime(&atime));
  ftime[strlen(ftime) - 1] = 0;
  printf("\t%s ", ftime);
  printf("\t%s ", name);

  if (S_ISLNK(mip->INODE.i_mode)) {
    readlink(name, linkbuf, 256);
    printf(" -> %s", linkbuf);
  }
  printf("\n");
}
/************************************************
 * Function: int ls_dir(MINODE *mip)
 * Programmer: Chandler Teigen
 * Description:
 * provides ls info for a single dir. adapted from algorithm
 * provided in (Wang, 330)
 * ***********************************************/
int ls_dir(MINODE *mip) {
  // printf("ls_dir: list CWD's file names; YOU do it for ls -l\n");

  char buf[BLKSIZE], temp[256];
  DIR *dp;
  char *cp;
  MINODE *m;
  // Assume DIR has only one data block i_block[0]
  get_block(dev, mip->INODE.i_block[0], buf);
  dp = (DIR *)buf;
  cp = buf;
  while (cp < buf + BLKSIZE) {
    strncpy(temp, dp->name, dp->name_len);
    temp[dp->name_len] = 0;
    m = iget(dev, dp->inode);
    // printf("[%d %s]  ", dp->inode, temp); // print [inode# name]

    ls_file(m, temp);
    iput(m);
    cp += dp->rec_len;
    dp = (DIR *)cp;
  }
  printf("\n");
}

/************************************************
 * Function: int ls(char *pathname)
 * Programmer: Chandler Teigen
 * Description:
 * implements the ls command for ext2 fs.
 * ***********************************************/
int ls(char *pathname) {
  if (pathname[0] == '\0') {
    strcpy(pathname, ".");
  }

  int ino = getino(pathname);
  MINODE *mip = iget(dev, ino);

  printf("ls %s\n", pathname);
  // printf("ls CWD only! YOU do it for ANY pathname\n");
  ls_dir(mip);
  iput(mip);
}

/************************************************
 * Function: char *pwd(MINOODE *wd)
 * Programmer: Chandler Teigen
 * Description:
 * implements the recursive part of the pwd algorithm
 * in (Wang, 330)
 * ***********************************************/
char *rpwd(MINODE *wd) {
  char buf[BLKSIZE];
  char my_name[EXT2_NAME_LEN];
  DIR *dp;
  char *cp;
  int my_ino, parent_ino;
  MINODE *pip;
  if (wd == root) {
    return 0;
  } else {
    my_ino = get_myino(wd, &parent_ino);
    pip = iget(dev, parent_ino);
    get_myname(pip, my_ino, my_name);
  }
}
/************************************************
 * Function: char *pwd(MINODE *wd)
 * Programmer: Chandler Teigen
 * Description:
 * either prints root symbol, or calls recursive pwd.
 * ***********************************************/
char *pwd(MINODE *wd) {
  if (wd == root) {
    printf("/\n");
  } else {
    rpwd(wd);
  }
  printf("\n");
}
