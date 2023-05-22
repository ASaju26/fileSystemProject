#ifndef _MFS_H
#define _MFS_H

#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <stdint.h>
#include "createfs_savefs.h"
#include "openfs_closefs.h"
#include "insert.h"
#include "readFile.h"
#include "encryptDecrypt.h"
#include "retrieve.h"
#include "delete.h"
#include "attrib.h"

// Stores data structures and global variables for filesystem

#define BLOCK_SIZE 1024
#define NUM_BLOCKS 65536
#define BLOCKS_PER_FILE 1024
#define MAX_NUM_FILES 256
#define FIRST_DATA_BLOCK 1364 // prev 790, 1001,790
#define MAX_FILE_SIZE 1048576

// Directory structure
struct _directoryEntry
{
    char filename[64];
    short inUse;
    int32_t inode;
};

// inode structure
struct inode
{
    int32_t blocks[BLOCKS_PER_FILE];
    short inUse;
    uint8_t attribute;
    uint32_t fileSize;
};

extern uint8_t data[NUM_BLOCKS][BLOCK_SIZE];
extern uint8_t *freeBlocks;                     // if freeBlocks[65536], then 512 blocks would be consumed just for free map
extern uint8_t *freeInodes;
extern FILE *fp;
extern char imageName[64];
extern uint8_t imageIsOpen;
extern struct inode *inodes;
extern struct _directoryEntry *directory;

void init();
uint32_t df();
int32_t findFreeBlock();
int32_t findFreeInode();
int32_t findFreeInodeBlock(int32_t inode);


#endif