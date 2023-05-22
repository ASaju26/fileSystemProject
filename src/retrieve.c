#include "retrieve.h"


void retrieve(char* imageFileName, char* newFileName)
{
    int offset  = 0;
    FILE* ofp = NULL;
    int32_t requestedInode = -1;
    // loop over the directory and find the filename
    for(int i=0;i<MAX_NUM_FILES;i++)
    {
        //printf("|%s|%s|\n",filename,directory[i].filename);//debug finding file
        if(strcmp(imageFileName,directory[i].filename)==0)
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
    int copy_size = inodes[requestedInode].fileSize;  
    //must start from beginning of inode array to retrieve entire file
    int inodeBlockIndex=0;
    int startingBlock= inodes[requestedInode].blocks[inodeBlockIndex];
    if(newFileName != NULL)
    {
        printf("Retrieving file from file system image and storing it in local directory file %s\n", newFileName);
        ofp = fopen(newFileName,"w");
    }
    else
    {
        printf("Retrieving file from file system image and storing it in local directory file %s\n",imageFileName);
        
        ofp = fopen(imageFileName,"w");
    }

        // Using copy_size as a count to determine when we've copied enough bytes to the output file.
        // Each time through the loop, except the last time, we will copy BLOCK_SIZE number of bytes from
        // our stored data to the file fp, then we will increment the offset into the file we are writing to.
        // On the last iteration of the loop, instead of copying BLOCK_SIZE number of bytes we just copy
        // how ever much is remaining ( copy_size % BLOCK_SIZE ).  If we just copied BLOCK_SIZE on the
        // last iteration we'd end up with gibberish at the end of our file. 
        while( copy_size > 0 )
        { 

        int num_bytes;

        // If the remaining number of bytes we need to copy is less than BLOCK_SIZE then
        // only copy the amount that remains. If we copied BLOCK_SIZE number of bytes we'd
        // end up with garbage at the end of the file.
        if( copy_size < BLOCK_SIZE )
        {
            num_bytes = copy_size;
        }
        else 
        {
            num_bytes = BLOCK_SIZE;
        }

        // Write num_bytes number of bytes from our data array into our output file.
        fwrite( data[startingBlock], num_bytes, 1, ofp ); 

        // Reduce the amount of bytes remaining to copy, increase the offset into the file
        // and increment the block_index to move us to the next data block.
        copy_size -= BLOCK_SIZE;
        offset    += BLOCK_SIZE;
        inodeBlockIndex++;
        startingBlock = inodes[requestedInode].blocks[inodeBlockIndex];

        // Since we've copied from the point pointed to by our current file pointer, increment
        // offset number of bytes so we will be ready to copy to the next area of our output file.
        fseek( ofp, offset, SEEK_SET );
        }

        // Close the output file, we're done. 
        fclose( ofp );
}

