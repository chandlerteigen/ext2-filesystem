int rm_child(MINODE *pmip, char *name) {
  char buf[BLKSIZE], *cp, temp[256];
  DIR *dp, *dp_previous;
  int found = 0, blk, deleted_rec_length = 0;
  // for each data block in the parent directory
  for (blk = 0; blk < pmip->INODE.i_blocks; blk++) {
    // get the current block
    get_block(dev, pmip->INODE.i_block[blk], buf);
    cp = buf;
    dp = (DIR *)buf;
    while (cp < buf + BLKSIZE) {
      strncpy(temp, dp->name, dp->name_len);
      temp[dp->name_len] = 0;
      // if the entry name is the same as the target name
      if (strcmp(name, temp) == 0) {
        found = 1;
        break;
      }
      dp_previous = dp;
      cp += dp->rec_len;
      dp = (DIR *)cp;
    }

    // break from the for loop if the name is found
    if (found) {
      // check if the rec_len is BLKSIZE (only entry in block)
      if (dp->rec_len == BLKSIZE) {
        // deallocate block
        bdalloc(dev, pmip->INODE.i_block[blk]);
        // shift data in i_blocks over
        // make sure blk isn't the last block in the array
        // and check to see if it has a zero entry next
        if (blk < EXT2_N_BLOCKS - 1 && pmip->INODE.i_block[blk + 1] != 0) {
          int blk_copy = blk;
          // keep looping until the end of the array
          while (blk_copy + 1 < EXT2_N_BLOCKS) {
            // shift the values in the array one to the "left"
            pmip->INODE.i_block[blk_copy] = pmip->INODE.i_block[blk_copy + 1];
            blk_copy++;
          }
        }

      }
      // now check if the entry is the last in the block
      // we know it is if the record goes all the way to the end of the block
      else if (buf + BLKSIZE == cp + dp->rec_len) {
        // absorb target directories rec_len in to the previous one.
        // this makes the previous entry the last in the block (deletes the
        // target)
        dp_previous->rec_len += dp->rec_len;
      }
      // the entry is either first, but not the only, or it is in the middle of
      // the block
      else {
        // move cp to point to the next entry
        cp += dp->rec_len;
        deleted_rec_length = dp->rec_len;

        while (cp < buf + BLKSIZE) {
          memcpy(dp, cp, ((DIR *)cp)->rec_len);
          cp = cp + dp->rec_len;
          dp_previous = dp;
          dp = (DIR *)((char *)dp + dp->rec_len);
        }
        dp_previous->rec_len += deleted_rec_length;
      }
      // since found is true, and the operation is complete, break out of the
      // for loop
      put_block(dev, pmip->INODE.i_block[blk], buf);
      break;
    }
  }

  if (found == 0) {
    printf("rmdir error: directory  %s not does not exist\n", name);
  }

  return deleted_rec_length;
}

/************************************************
 * Function: ct_rmdir
 * Programmer: Chandler Teigen
 * Description: takes in a pathname, check if it is
 * an empty directory and is not in use, then removes 
 * it.
 * ***********************************************/
int ct_rmdir(char *pathname) {
  int ino, pino, i, entries;
  MINODE *mip, *pmip;
  char myname[100], buf[BLKSIZE], dir[100], base[100];
  DIR *dp;
  char *cp;

  ino = getino(pathname);
  mip = iget(dev, ino);

  // check if the INODE is a directory
  if (S_ISDIR(mip->INODE.i_mode)) {
    // check that the INODE is not busy
    if (mip->refCount == 1) {
      // check that the directory is busy by first checking its links count
      if (mip->INODE.i_links_count > 2) {
        printf("rmdir error: %s is not empty\n", pathname);
      } else {
        i = entries = 0;
        // i_blocks / 2 because the i_blocks field is using blk size 512?
        while (i < mip->INODE.i_blocks / 2 && entries < 3) {
          get_block(dev, mip->INODE.i_block[i], buf);
          cp = buf;
          dp = (DIR *)buf;
          // iterate over the directory entries in the block and count them,
          while (cp < buf + BLKSIZE && entries < 3) {
            printf("name: %s\n", dp->name);
            entries++;
            cp += dp->rec_len;
            dp = (DIR *)cp;
          }

          i++;
        }
        if (entries <= 2) {
          // get parent info
          int myino;
          pino = findino(mip, &myino);
          pmip = iget(dev, pino);

          // grab file name from the parent info
          // findmyname(pmip, ino, myname);
          divide_pathname(pathname, dir, base);

          rm_child(pmip, base);

          // decrement links count, mark as dirty and put the changes back to
          // the block
          pmip->INODE.i_links_count--;
          pmip->dirty = 1;
          iput(pmip);

          bdalloc(dev, mip->INODE.i_block[0]);
          idalloc(dev, mip->ino);
          // do I need to mark mip as dirty in one of the above??
          iput(mip);
        } else {
          printf("rmdir error: %s is not empty\n", pathname);
        }
      }
    } else {
      printf("rmdir error: %s is busy\n", pathname);
    }
  } else {
    printf("rmdir error: %s is not a directory", pathname);
  }
}