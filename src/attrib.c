#include "attrib.h"

void attrib(char *filename, char *change)
{
    // 0x1 = hidden
    // 0x2 = read-only

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
    if (requestedInode == -1)
    {
        printf("ERROR: File not found\n");
        return;
    }
    if (strcmp(change,"+h") == 0)
    {
        // If file is not hidden
        if ((inodes[requestedInode].attribute & HIDDEN) != HIDDEN)
        {
            inodes[requestedInode].attribute = inodes[requestedInode].attribute ^ HIDDEN;
            return;
        }
        else    // Else, file is already hidden
        {
            printf("ERROR: File already has hidden attribute.\n");
            return;
        }
        
    }
    else if (strcmp(change, "+r") == 0)
    {
        // If file is not read-only
        if ((inodes[requestedInode].attribute & READONLY) != READONLY)
        {
            inodes[requestedInode].attribute = inodes[requestedInode].attribute ^ READONLY;
            return;
        }
        else    // else, file is read-only already
        {
            printf("ERROR: File already has read-only attribute.\n");
            return;
        }
    }
    else if (strcmp(change, "-h") == 0)
    {
        // If file is hidden
        if ((inodes[requestedInode].attribute & HIDDEN) == HIDDEN)
        {
            inodes[requestedInode].attribute = inodes[requestedInode].attribute ^ HIDDEN;
            return;
        }
        else    // else, file is not hidden
        {
            printf("ERROR: No hidden attribute to remove. File is not hidden.\n");
            return;
        }
        
    }
    else if (strcmp(change, "-r") == 0)
    {
        // If file is read-only
        if ((inodes[requestedInode].attribute & READONLY) == READONLY)
        {
            inodes[requestedInode].attribute = inodes[requestedInode].attribute ^ READONLY;
            return;
        }
        else    // else, file is not read-only
        {
            printf("ERROR: No read-only attribute to remove. File is not read-only.\n");
            return;
        }
    }
    else
    {
        printf("ERROR: Command not found.\n");
        return;
    }
}