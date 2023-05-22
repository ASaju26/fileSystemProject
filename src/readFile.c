#include "readFile.h"

void readFile(char * filename, size_t start, size_t numBytes)
{
int32_t requestedInode = -1;
// loop over the directory and find the filename
for(int i=0;i<MAX_NUM_FILES;i++)
{
    //printf("|%s|%s|\n",filename,directory[i].filename);//debug finding file
    if(strcmp(filename,directory[i].filename)==0)
    {
        //   get the inode of the file
        requestedInode = directory[i].inode;
        i = MAX_NUM_FILES;
    }
}
if(requestedInode == -1)
{
    printf("ERROR: File not found\n");
    return;
}
//
// the starting block that you need is start / 1024
int remainder = start%1024;
int inodeBlockIndex=start/1024;
int startingBlock= inodes[requestedInode].blocks[inodeBlockIndex];
// save that block number
// declare a uint8_t pointer called ptr and set it data[block_number][remainder]
uint8_t * ptr = &data[startingBlock][remainder];
// loop and print the number of bytes
for(int j = 0;j<numBytes;j++)
{
    if(remainder < BLOCK_SIZE)
    {
        printf(" 0x%x",*ptr);
        remainder++;
        ptr = &data[startingBlock][remainder];
    }
    else
    {
        printf(" 0x%x\n",*ptr);
        inodeBlockIndex++;
        startingBlock = inodes[requestedInode].blocks[inodeBlockIndex];
        remainder = 0;
        ptr  =&data[startingBlock][remainder];
    } 
}
printf("\n");
return;
}