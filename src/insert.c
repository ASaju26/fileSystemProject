#include "insert.h"


void insert(char* filename)
{
    // verify the filename is not NULL
    if (filename == NULL)
    {
        printf("ERROR: Filename is NULL\n");

        return;
    }

    // verify the file exists
    struct stat buff;
    int ret = stat(filename, &buff);

    if (ret == -1)
    {
        printf("ERROR: File does not exist\n");

        return;
    }

    // verify the file is not too big
    if (buff.st_size > MAX_FILE_SIZE)
    {
        printf("ERROR: File size exceeds maximum size\n");

        return;
    }

    // verify that there is enough space
    if (buff.st_size > df())
    {
        printf("ERROR: Not enough free disk space");
    }

    // find empty directory entry
    int directoryEntryFound = -1;
    for (int i = 0; i < MAX_NUM_FILES; i++)
    {
        if (directory[i].inUse == 0)
        {
            directoryEntryFound = i;
            break;
        }
    }

    if (directoryEntryFound == -1)
    {
        printf("ERROR: Could not find a free directory entry\n");

        return;
    }

    FILE *ifp = fopen(filename, "r");
    printf("Reading %d bytes from %s\n", (int)buff.st_size, filename);

    int32_t copySize = buff.st_size;
    int32_t offset = 0;
    int32_t blockIndex = -1;

    // find a free inode
    int32_t inodeIndex = findFreeInode();
    if (inodeIndex == -1)
    {
        printf("ERROR: Cannot find a free inode\n");

        return;
    }
    freeInodes[inodeIndex] = 0;

    // place the file info in the directory
    directory[directoryEntryFound].inUse = 1;
    directory[directoryEntryFound].inode = inodeIndex;
    strncpy(directory[directoryEntryFound].filename, filename, strlen(filename));

    inodes[inodeIndex].fileSize = buff.st_size;

    while (copySize > 0)
    {
        fseek(ifp, offset, SEEK_SET);

        // find a free block
        blockIndex = findFreeBlock();
        if (blockIndex == -1)
        {
            printf("ERROR: Cannot find a free block\n");

            return;
        }

        freeBlocks[blockIndex] = 0;

        int32_t bytes = fread(data[blockIndex], BLOCK_SIZE, 1, ifp);

        // save the block in the inode
        int32_t inodeBlock = findFreeInodeBlock(inodeIndex);
        inodes[inodeIndex].blocks[inodeBlock] = blockIndex;

        if( bytes == 0 && !feof( ifp ) )
        {
            printf("ERROR: An error occured reading from the input file\n");

            return;
        }

        clearerr(ifp);

        copySize -= BLOCK_SIZE;
    
        offset += BLOCK_SIZE;

        blockIndex = findFreeBlock();
    }

    fclose(ifp);
}