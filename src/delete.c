#include "delete.h"

void delete(char *fileName)    //takes in file struct pointer which should hold info on the file
{
    int i = 0;          //variable used to help traverse through the entries

    while(directory)    //This will allow me to traverse through the directories while it exists or is not NULL
    {
        if(directory[i].inUse)    //This will go into the block if the entry/file is in use
        {
            if(!strcmp(fileName, directory[i].filename))   //will only go into if block if the filename is a match
            {
                directory[i].inUse = 0;   //Changes the InUse to off so there can be no access to the file, but not saved yet
                inodes[directory[i].inode].inUse = 0;
                freeInodes[directory[i].inode] = 1;

                for (int j = 0; j < BLOCKS_PER_FILE; j++)
                {
                    if (freeBlocks[inodes[directory[i].inode].blocks[j]] == 1)
                    {
                        break;
                    }
                    else
                    {
                        freeBlocks[inodes[directory[i].inode].blocks[j]] = 1;
                    }
                }
                
                break;
            }
        }
        i++;
    }
}

void undel(char *filename)
{
    int i = 0;

    while (directory != NULL)
    {
        if (strcmp(filename, directory[i].filename) == 0)
        {
            directory[i].inUse = 1;
            inodes[directory[i].inode].inUse = 1;
            freeInodes[directory[i].inode] = 0;

            for (int j = 0; j < BLOCKS_PER_FILE; j++)
            {
                if (freeBlocks[inodes[directory[i].inode].blocks[j]] == 0)
                {
                    break;
                }
                else
                {
                    freeBlocks[inodes[directory[i].inode].blocks[j]] = 0;
                }
            }
                
            break;
        }
        i++;
    }
}
