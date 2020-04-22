
/************************************************
 * Function: ct_cp
 * Programmer: Chandler Teigen
 * Description: reads the contents of the source file
 * and writes them to the destination file.
 * ***********************************************/
int ct_cp(char *srcpath, char *destpath) {
  int fd, gd, n;
  char buf[BLKSIZE];
  fd = ct_open(srcpath, 0);  // open the source file for read
  gd = ct_open(destpath, 1); // open dest file for write

  while (n = ct_read(fd, buf, BLKSIZE)) {
    ct_write(gd, buf, n);
  }

  ct_close(fd);
  ct_close(gd);
}

/************************************************
 * Function: ct_write
 * Programmer: Chandler Teigen
 * Description: writes nbytes bytes from the memory
 * pointed to by buf into the file indicated by the
 * file descriptor.
 * ***********************************************/
int ct_write(int fd, char *buf, int nbytes) {

  int lbk, blk, start_byte, count = 0;
  OFT *oftp = running->fd[fd];
  int ibuf[256], dibuf[256], remain, min;
  char writebuf[BLKSIZE];
  char *cq = buf;

  lbk = oftp->offset / BLKSIZE;
  start_byte = oftp->offset % BLKSIZE;

  while (nbytes > 0) {
    if (lbk < 12) {                              // direct block
      if (oftp->mptr->INODE.i_block[lbk] == 0) { // if no data block yet
        oftp->mptr->INODE.i_block[lbk] =
            balloc(oftp->mptr->dev); // MUST ALLOCATE a block
      }
      blk = oftp->mptr->INODE.i_block[lbk];   // blk should be a disk block now
    } else if (lbk >= 12 && lbk < 256 + 12) { // INDIRECT blocks
      // HELP INFO:
      if (oftp->mptr->INODE.i_block[12] == 0) {
        oftp->mptr->INODE.i_block[12] = balloc(oftp->mptr->dev);
      }
      get_block(oftp->mptr->dev, oftp->mptr->INODE.i_block[12], ibuf);
      // get i_block[12] into an int ibuf[256];
      blk = ibuf[lbk - 12];
      if (blk == 0) {
        blk = balloc(oftp->mptr->dev);
        ibuf[lbk - 12] = blk;
        put_block(oftp->mptr->dev, oftp->mptr->INODE.i_block[12], ibuf);
        // allocate a disk block;
        // record it in i_block[12];
      }
    } else {
      // if the double indirect block doesn't exist, alloc it
      if (oftp->mptr->INODE.i_block[13] == 0) {
        oftp->mptr->INODE.i_block[13] = balloc(oftp->mptr->dev);
      }
      // get the double indirect block
      get_block(oftp->mptr->dev, oftp->mptr->INODE.i_block[13], dibuf);

      // if the indirect block doesn't exist, alloc it
      if (dibuf[(lbk - 12 - 256) / 256] == 0) {
        dibuf[(lbk - 12 - 256) / 256] = balloc(oftp->mptr->dev);
        // make sure to save the changes to the di block
        put_block(oftp->mptr->dev, oftp->mptr->INODE.i_block[13], dibuf);
      }
      // get the indirect block
      get_block(oftp->mptr->dev, dibuf[(lbk - 12 - 256) / 256], ibuf);

      if (ibuf[(lbk - 12 - 256) % 256] == 0) {
        // alloc a new block
        ibuf[(lbk - 12 - 256) % 256] = balloc(oftp->mptr->dev);
        // save the changes to the indirect block
        put_block(oftp->mptr->dev, dibuf[(lbk - 12 - 256) / 256], ibuf);
      }

      blk = ibuf[(lbk - 12 - 256) % 256];
    }

    // get the  block into the buf
    get_block(oftp->mptr->dev, blk, writebuf);

    char *cp = writebuf + start_byte;
    remain = BLKSIZE - start_byte;

    while (remain > 0) {
      if (nbytes > remain) {
        min = remain;
      } else {
        min = nbytes;
      }

      memcpy(cp, cq, min);
      nbytes -= min;
      remain -= min;
      oftp->offset += min;
      count += min;
      if (oftp->offset > oftp->mptr->INODE.i_size) {
        oftp->mptr->INODE.i_size += (oftp->offset - oftp->mptr->INODE.i_size);
      }
      if (nbytes <= 0) {
        break;
      }
    }

    put_block(oftp->mptr->dev, blk, writebuf);
  }

  oftp->mptr->dirty = 1;
  // printf("wrote %d char into fd=%d\n", count, fd);
  return nbytes;
}

/************************************************
 * Function: write_file
 * Programmer: Chandler Teigen
 * Description: Prompts the user for a file descriptor
 * and a string to write to the file, then it calls
 * ct_write.
 * ***********************************************/
int write_file() {
  char buf[BLKSIZE], nl;
  int fd, nbytes;
  printf("Please enter an fd and a string to write to the file: ");

  scanf("%d %s", &fd, &buf);
  scanf("%c", &nl);

  nbytes = strlen(buf);

  if (running->fd[fd] != NULL &&
      (running->fd[fd]->mode == 1 || running->fd[fd]->mode == 2 ||
       running->fd[fd]->mode == 3)) {
    return ct_write(fd, buf, nbytes);
  } else {
    printf("write error: fd is not open, or not open for write\n");
    return -1;
  }
}