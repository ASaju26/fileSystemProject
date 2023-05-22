#include "encryptDecrypt.h"

void encryptDecrypt(char *filename, char key)
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
    int location = 0;
    int inodeBlockIndex=0;
    int startingBlock= inodes[requestedInode].blocks[inodeBlockIndex];
// save that block number
// declare a uint8_t pointer called ptr and set it data[block_number][remainder]
//uint8_t * ptr = &data[startingBlock][location];
// loop and print the number of bytes
for(int j = 0;j<inodes[requestedInode].fileSize;j++)//change later
{
    if(location < BLOCK_SIZE)
    {
        data[startingBlock][location] = data[startingBlock][location] ^ key;
        location++;
    }
    else
    {
        data[startingBlock][location] = data[startingBlock][location] ^ key;
        inodeBlockIndex++;
        startingBlock = inodes[requestedInode].blocks[inodeBlockIndex];
        location = 0;

    } 
}
    
return;

}