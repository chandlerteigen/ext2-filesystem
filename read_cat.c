/*********************************************
 * Programmer: Chandler Teigen
 * Date: 5/31/2020
 * Description:
 * Contains the functions for the read and cat commands,
 * plus related functions.
 * *******************************************/

#include "read_cat.h"

int find_min(int num1, int num2, int num3) {
  int min = num1;
  if (num2 < min) {
    min = num2;
  }
  if (num3 < min) {
    min = num3;
  }
  return min;
}

int cat(char *pathname) {
  char buf[BLKSIZE + 1], dummy = 0;
  int n;

  int fd = ct_open(pathname, 0); // open file for read mode
  if (fd >= 0) {
    while (n = ct_read(fd, buf, BLKSIZE)) {
      buf[n] = 0;
      printf("%s\n", buf);
    }
    ct_close(fd);
  }
}

int ct_read(int fd, char *buf, int nbytes) {
  OFT *oftp = running->fd[fd];
  MINODE *mip = oftp->mptr;
  int count = 0, lbk, blk, start_byte;
  int avail = mip->INODE.i_size - oftp->offset;
  char *cq = buf;
  int ibuf[256], dibuf[256];
  char readbuf[1024];

  // loop while nbytes and avail are not equal to zero
  while (nbytes && avail) {
    // find lbk number
    lbk = oftp->offset / BLKSIZE;
    // find start_byte
    start_byte = oftp->offset % BLKSIZE;

    // lbk is direct block
    if (lbk < 12) {
      blk = mip->INODE.i_block[lbk];
    }
    // lbk is an indirect block
    else if (lbk >= 12 && lbk < 256 + 12) {
      get_block(dev, mip->INODE.i_block[12], (char *)ibuf);
      blk = ibuf[lbk - 12];
    } else {
      // get the double indirect block
      get_block(dev, mip->INODE.i_block[13], dibuf);
      // get the indirect block
      get_block(dev, dibuf[(lbk - 12 - 256) / 256], ibuf);
      // compute the  blk number of the direct block
      blk = ibuf[(lbk - 12 - 256) % 256];
    }

    // get the blk into readbuf
    get_block(dev, blk, readbuf);

    char *cp = readbuf + start_byte;
    int remain = BLKSIZE - start_byte;

    while (remain > 0) {
      int min = find_min(remain, avail, nbytes);
      memcpy(cq, cp, min);
      cp += min;
      cq += min;
      oftp->offset += min;
      count += min;
      avail -= min;
      nbytes -= min;
      remain -= min;
      if (nbytes <= 0 || avail <= 0) {
        break;
      }
    }
  }
  // printf("ct_read: read %d char from fd %d\n", count, fd);
  return count;
}

int read_file() {
  int fd, nbytes, bytes_read;
  char buf[BLKSIZE], nl;
  printf("Please enter fd and nbytes: ");

  scanf("%d%d", &fd, &nbytes);
  scanf("%c", &nl);
  // check that the file is open and opened for either R or RW

  if (running->fd[fd] != NULL &&
      (running->fd[fd]->mode == 0 || running->fd[fd]->mode == 2)) {
    bytes_read = ct_read(fd, buf, nbytes);
    printf("%s\n", buf);
    return bytes_read;
  } else {
    printf("read error: fd is either not open, or not open for Read or "
           "Read/Write\n");
  }
}