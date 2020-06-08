/*********************************************
 * Programmer: Chandler Teigen
 * Date: 5/31/2020
 * Description:
 * Contains the functions for the open, close,
 * and lseek commands, and related functions.
 * *******************************************/

#include "open_close_lseek.h"
#include "util.h"
#include "mkdir_creat.h"

int ct_truncate(MINODE *mip) {
  char buf[BLKSIZE], buf2[BLKSIZE], buf3[BLKSIZE];
  int blk, blk2;

  // iterate over the direct blocks, and clear them
  for (int i = 0; i < 11; i++) {
    if (mip->INODE.i_block[i] != 0) // make sure it points to an actual block
    {
      get_block(dev, mip->INODE.i_block[i], buf);
      memset(buf, 0, BLKSIZE);
      put_block(dev, i, buf);
    }
  }

  if (mip->INODE.i_block[12] != 0) {
    // get indirect block #12 into buf
    get_block(dev, mip->INODE.i_block[12], buf);
    // iterate over each block number in the indirect block
    for (int i = 0; i < BLKSIZE; i += 4) {
      blk = (int)(buf + i);
      // get each block pointed to in the indirect block, set it to 0s
      if (blk != 0) {
        get_block(dev, blk, buf2);
        memset(buf2, 0, BLKSIZE);
        put_block(dev, blk, buf2);
      }
    }
  }
  if (mip->INODE.i_block[13] != 0) {
    // get the double indirect block
    get_block(dev, mip->INODE.i_block[13], buf);

    for (int i = 0; i < BLKSIZE; i += 4) {
      blk = (int)(buf + i);

      if (blk != 0) {
        // get the indirect block
        get_block(dev, blk, buf2);
        // iterate over each block pointed to by the indirect block
        for (int j = 0; j < BLKSIZE; j += 4) {
          blk2 = (int)(buf2 + 4);
          if (blk2 != 0) {
            // get the direct block and set it to 0s
            get_block(dev, blk2, buf3);
            memset(buf3, 0, BLKSIZE);
            put_block(dev, blk2, buf3);
          }
        }
      }
    }
  }

  // update INODEs time field
  // am I settting the correct times here??? TODO
  mip->INODE.i_mtime = mip->INODE.i_atime = time(0L);

  // mark dirty
  mip->dirty = 1;
}

int open_file()
{
  char pathname[100];
  int mode;
  char nl;
  printf("Please enter a pathname and an open mode (0 1 2 3 -> R W RW A): ");
  scanf("%s %d", pathname, &mode);
  scanf("%c", &nl); // grab the new line character...
  printf("\n");
  ct_open(pathname, mode);
}

int ct_open(char *pathname, int flags) {
  int ino, permission_ok = 0, fd, i, open_okay;
  MINODE *mip;

  ino = getino(pathname);

  if (ino == 0) // file does not exist
  {
    ct_creat_file(pathname);
    ino = getino(pathname);
  }

  if (pathname[0] == '/') {
    dev = root->dev;
  } else {
    dev = running->cwd->dev;
  }

  mip = iget(dev, ino);

  if (S_ISREG(mip->INODE.i_mode)) {
    if (flags == 0 && mip->INODE.i_mode & (1 << 8)) // check owner read
                                                    // permissions if the open
                                                    // request is for mode read
    {
      permission_ok = 1;
    } else if ((flags == 1 || flags == 3) &&
               mip->INODE.i_mode & (1 << 7)) // check write permissions if
                                             // opening for write or append
    {
      permission_ok = 1;
    } else if (flags == 2 && mip->INODE.i_mode & (1 << 8) &&
               mip->INODE.i_mode &
                   (1 << 7)) // check for read and write permissions
    {
      permission_ok = 1;
    }

    // check if the file is already open for anything but read

    // a MINODE* is the same as another if it has the same dev and the same
    // ino... for future reference.
    open_okay = 1;
    for (int i = 0; i < NFD; i++) {
      // Check if the file is already open in an fd
      if (running->fd[i] != 0 && running->fd[i]->mptr->dev == mip->dev &&
          running->fd[i]->mptr->ino == mip->ino) {
        if (running->fd[i]->mode !=
            0) // check if it is anything but read. multiple writes not allowed
        {
          open_okay = 0;
        }
      }
    }

    // allocate a free OFT and fill in the values
    if (open_okay) {
      OFT *oftp = (OFT *)malloc(sizeof(OFT));
      oftp->mode = flags;
      oftp->refCount = 1;
      oftp->mptr = mip;

      // depending on the open mode, set the offset to the correct value
      switch (flags) {
      case 0:
        oftp->offset = 0; // Read: offset = 0
        break;
      case 1:
        ct_truncate(mip);
        oftp->offset = 0; // Write: truncate file and offset = 0
        break;
      case 2:
        oftp->offset = 0; // Read/Write: offset = 0, not truncate
        break;
      case 3:
        oftp->offset = mip->INODE.i_size; // Append: offset = file size
        break;
      default:
        printf("open error: mode is invalid\n");
        return -1;
      }

      // find the smallest i such that running->fd[i] == NULL

      for (fd = 0; fd < NFD; fd++) {
        if (running->fd[fd] == NULL) {
          break;
        }
      }

      // if a NULL fd was not found
      if (fd == NFD) {
        printf("open error: no free file descriptors\n");
        return -1;
      } else {
        running->fd[fd] = oftp;
      }

      if (flags == 0) {
        // for read mode, touch atime
        mip->INODE.i_atime = time(0L);
      } else {
        // for any other mode, touch atime and mtime
        mip->INODE.i_atime = mip->INODE.i_mtime = time(0L);
      }
      mip->dirty = 1;

      return fd;
    } else {
      printf("open error: the file is already open for a mode other than read\n");
      return -1;
    }
  } else {
    printf("open error: %s is not a regular file.\n");
    return -1;
  }
}

int close_file()
{
  int fd;
  char nl;
  printf("Please enter an fd to close: ");
  scanf("%d", &fd);
  scanf("%c", &nl); // get the trailing new line
  printf("\n");
  ct_close(fd);

}

int ct_close(int fd) {
  OFT *oftp;

  // verify that fd is within the correct range
  if (fd >= 0 && fd < NFD) {
    // verify that running->fd[fd] points to a oft entry
    if (running->fd[fd] != NULL) {
      oftp = running->fd[fd];
      running->fd[fd] = 0; // reset the fd[i] to NULL
      oftp->refCount--;

      // check if the OFT entry is still in use
      if (oftp->refCount > 0) {
        return 0;
      } else {
        // release the inode pointer, and the memory that was allocated for the OFT struct
        iput(oftp->mptr);
        free(oftp);
      }

      return 0;
    } else {
      printf("close error: fd does not correspond to a OFT entry\n");
      return -1;
    }

  } else {
    printf("close error: fd is not within the correct range\n");
    return -1;
  }
}

int lseek_file()
{
  int fd, position;
  char nl;
  printf("Please enter an fd and a byte position: ");
  scanf("%d %d", &fd, &position);
  scanf("%c", &nl);
  printf("\n");
  ct_lseek(fd, position);
}

int ct_lseek(int fd, int position) {
  OFT *oftp;
  int original_position;
  // verift the fd is within range
  if (fd >= 0 && fd < NFD) {
    oftp = running->fd[fd];

    // make sure that the position is within the file ends
    if (position >= 0 && position <= oftp->mptr->INODE.i_size) {
      original_position = oftp->offset;
      oftp->offset = position;
      return original_position;
    } else {
      printf("lseek error: position is not within the file's size\n");
      return -1;
    }

  } else {
    printf("lseek error: fd is out of range\n");
    return -1;
  }
}

int pfd() {
  char modes[4][12] = {"READ", "WRITE", "RE/WR", "APND"};

  printf("fd      mode    offset  INODE\n");
  for (int i = 0; i < NFD; i++) {
    // if the fd is not NULL, print it
    if (running->fd[i] != NULL) {
      printf("%d\t%s\t%d\t[%d,%d]\n", i, modes[running->fd[i]->mode],
             running->fd[i]->offset, running->fd[i]->mptr->dev,
             running->fd[i]->mptr->ino);
    }
  }
}

int dup(int fd) {
  int i;

  if (running->fd[fd] != NULL) {
    i = 0;
    while (running->fd[i] != NULL) {
      i++;
    }
    if (i < NFD) {
      running->fd[i] = running->fd[fd];
      running->fd[fd]->refCount++;
    } else {
      printf("dup error: no open file descriptors\n");
    }

  } else {
    printf("dup error: fd is not open\n");
  }
}

int dup2(int fd, int gd) {
  // if gd is already open, close it
  if (running->fd[gd] != NULL) {
    ct_close(gd);
  }

  // verify that fd is actually open, then duplucate it into gd
  if (running->fd[fd] != NULL) {
    running->fd[gd] = running->fd[fd];
    running->fd[fd]->refCount++;
  }
}