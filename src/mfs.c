#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <stdint.h>

#include "mfs.h"

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
#define MAX_COMMAND_SIZE 255    // The maximum command-line size
#define MAX_NUM_ARGS 11


uint8_t data[NUM_BLOCKS][BLOCK_SIZE];

// if freeBlocks[65536], then 512 blocks would be consumed just for free map
uint8_t *freeBlocks;
uint8_t *freeInodes;

struct _directoryEntry *directory;

FILE *fp;
char imageName[64];
uint8_t imageIsOpen;
struct inode *inodes;

char* getInput(char *commandString)
{
    do
    {
        strcpy(commandString, "");
        printf("mfs> ");
        while (!fgets(commandString, MAX_COMMAND_SIZE, stdin));
    } while (commandString[0] == '\n');

    return commandString;
}

void init()
{
    directory = (struct _directoryEntry*)&data[0][0];
    inodes = (struct inode*)&data[84][0];
    freeBlocks = (uint8_t*)&data[19][0]; // prev 277, 1000
    freeInodes = (uint8_t*)&data[18][0];

    for (int i = 0; i < MAX_NUM_FILES; i++)
    {
        directory[i].inUse = 0;
        directory[i].inode = -1;
        freeInodes[i] = 1;
        memset(directory[i].filename, 0, 64);

        for (int j = 0; j < NUM_BLOCKS; j++)
        {
            inodes[i].blocks[j] = -1;
            inodes[i].inUse = 0;
            inodes[i].attribute = 0;
            inodes[i].fileSize = 0;
        }
    }

    for (int i = 0; i < NUM_BLOCKS; i++)
    {
        freeBlocks[i] = 1;
    }

    imageIsOpen = 0;
}

uint32_t df()
{
    int count = 0;

    for (int i = FIRST_DATA_BLOCK; i < NUM_BLOCKS; i++)
    {
        if (freeBlocks[i] == 1)
        {
            count++;
        }
    }

    return count * BLOCK_SIZE;
}

void ConvertDecimalToBinary(int a, char binString[8 + 1])
{
    memset(binString, 0, 8);
    int conversion[8] = {};
    conversion[0] = a;

    for (int i = 1; i < 8; i++)
    {
        a = a>>1;
        conversion[i] = a;  
    }
    for (int i = 0; i < 8; i++)
    {
        conversion[i] = conversion[i] & 1;
    }
    for (int i = 8 - 1; i >= 0; i--)
    {
        binString[8 - 1 - i] = conversion[i] + '0';
    }
    binString[8] = '\0';
}

void list(uint8_t option)
{
    int fileNotFound = 1;
    for (int i = 0; i < MAX_NUM_FILES; i++)
    {
        // TODO: Add check to not list hidden files
        if (directory[i].inUse == 1)
        {
            fileNotFound = 0;
            char filename[65];
            memset(filename, 0, 65);
            strncpy(filename, directory[i].filename, strlen(directory[i].filename));
            if (option == 0)
            {
                if ((inodes[directory[i].inode].attribute & 0x1) != 0x1)
                {
                    printf("    %s\n", filename);
                }
            }
            if (option == 1)    // print with hidden files
            {
                printf("    %s\n", filename);
            }
            if (option == 2)    // print with file attributes
            {
                printf("    %-30s", filename);
                char binString[8];
                ConvertDecimalToBinary(inodes[directory[i].inode].attribute, binString);
                printf("  %9s\n", binString);
                

            }
        }
    }

    if (fileNotFound == 1)
    {
        printf("ERROR: No files found\n");
    }
}

int32_t findFreeBlock()
{
    for (int i = FIRST_DATA_BLOCK; i < NUM_BLOCKS; i++)
    {
        if (freeBlocks[i])
        {
            return i + 1364; //prev 790, 1001,790
        }
    }

    return -1;
}

int32_t findFreeInode()
{
    for (int i = 0; i < MAX_NUM_FILES; i++)
    {
        if (freeInodes[i])
        {
            return i;
        }
    }

    return -1;
}

int32_t findFreeInodeBlock(int32_t inode)
{
    for (int i = 0; i < BLOCKS_PER_FILE; i++)
    {
        if (inodes[inode].blocks[i] == -1)
        {
            return i;
        }
    }

    return -1;
}

int main()
{
    char *commandString = (char*)malloc(MAX_COMMAND_SIZE * sizeof(char));
    char *token[MAX_NUM_ARGS];

    memset(imageName, 0, 64);

    fp = NULL;

    init();

    while (1)
    {
        getInput(commandString);

        // Clear out token array
        for (int i = 0; i < MAX_NUM_ARGS; i++)
        {
            token[i] = NULL;
        }

        int tokenCount = 0;

        // Pointer to point to the token
        // parsed by strsep
        char *argumentPtr = NULL;

        char *workingString = strdup(commandString);

        // we are going to move the working_string pointer so
        // keep track of its original value so we can deallocate
        // the correct amount at the end
        char *headPtr = workingString;

        // Tokenize the input strings with whitespace used as the delimiter
        while (((argumentPtr = strsep(&workingString, WHITESPACE ) ) != NULL) && 
            (tokenCount<MAX_NUM_ARGS))
        {
            token[tokenCount] = strndup(argumentPtr, MAX_COMMAND_SIZE);
            if(strlen(token[tokenCount]) == 0)
            {
                token[tokenCount] = NULL;
            }
            tokenCount++;
        }


        // Process filesystem commands
        if (strcmp(token[0], "createfs") == 0)
        {
            if (token[1] == NULL)
            {
                printf("ERROR: No filename specified\n");
                continue;
            }

            createfs(token[1]);
        }

        if (strcmp(token[0], "savefs") == 0)
        {
            savefs();
        }

        if (strcmp(token[0], "open") == 0)
        {
            if (token[1] == NULL)
            {
                printf("ERROR: No filename specified\n");
                continue;
            }

            openfs(token[1]);
        }

        if (strcmp(token[0], "close") == 0)
        {
            closefs();
        }

        if (strcmp(token[0], "list") == 0)
        {
            if (imageIsOpen == 0)
            {
                printf("ERROR: Disk image is not open\n");
                continue;
            }

            if ((token[1] != NULL) && (strcmp(token[1], "-h")) == 0)
            {
                list(1);
            }
            else if ((token[1] != NULL) && (strcmp(token[1], "-a")) == 0)
            {
                list(2);
            }
            else
            {
               list(0); 
            }
            
        }
        if (strcmp(token[0], "retrieve") == 0)
        {
            if (imageIsOpen == 0)
            {
                printf("ERROR: Disk image is not open\n");
                continue;
            }

            if (token[1] != NULL)
            {
                if (token[2]!=NULL)
                {
                    retrieve(token[1],token[2]);
                }
                else
                {
                    retrieve(token[1],NULL);
                }
            }
            else
            {
                printf("Error: No file name entered for retrieval\n");
            }
          
        }

        if (strcmp(token[0], "df") == 0)
        {
            if (imageIsOpen == 0)
            {
                printf("ERROR: Disk image is not open\n");
                continue;
            }

            printf("%d bytes free\n", df());
        }

        if (strcmp(token[0], "insert") == 0)
        {
            if (imageIsOpen == 0)
            {
                printf("ERROR: Disk image is not open\n");
                continue;
            }

            if (token[1] == NULL)
            {
                printf("ERROR: No filename specified\n");
                continue;
            }

            insert(token[1]);
        }
        if(strcmp(token[0],"read") == 0)
        {
            if (imageIsOpen == 0)
            {
                printf("ERROR: Disk image is not open\n");
                continue;
            }
            if(token[1] != NULL)
            {
                
                if(token[2]!=NULL)
                {
                    if(token[3]!=NULL)
                    {
                        readFile(token[1],atoi(token[2]),atoi(token[3]));
                    }
                    else
                    {  
                        printf("Error: Please specify the number of bytes to read\n");
                        continue;
                    }
                }
                else
                {
                    printf("ERROR: Please specify the starting byte\n");
                    continue;
                }

            }
            else
            {
                printf("ERROR: Please specify the file name\n");
                continue;
            }   
        }

        if (strcmp(token[0],"encrypt") == 0)
        {
            if (imageIsOpen == 0)
            {
                printf("ERROR: Disk image is not open\n");
                continue;
            }

            if(token[1] == NULL)
            {
                printf("ERROR: No filename specified\n");
                continue;
            }

            if(token[2] == NULL)
            {
                printf("ERROR: No cipher specified\n");
                continue;
            }
            char key = atoi( token[2] );
            encryptDecrypt(token[1], key);

        }
        if (strcmp(token[0],"attrib")==0)
        {
            if (imageIsOpen == 0)
            {
                printf("ERROR: Disk image is not open\n");
                continue;
            }

            if(token[1] != NULL)
            {
                if(token[2]!=NULL)
                {
                    attrib(token[2],token[1]);
                }
            
                else
                {
                    printf("ERROR: No filename entered\n");
                    continue;
                }
            }
            else
            {
                printf("ERROR: No attribute change specified\n");
                continue;
            }

        }

        if(strcmp(token[0], "delete") == 0)
        {
            if (imageIsOpen == 0)
            {
                printf("ERROR: Disk image is not open\n");
                continue;
            }

            if(token[1] == NULL)
            {
                printf("ERROR: No filename specified\n");
                continue;
            }

            delete(token[1]);
        }
         if(strcmp(token[0], "undelete") == 0)
        {
            if (imageIsOpen == 0)
            {
                printf("ERROR: Disk image is not open\n");
                continue;
            }

            if(token[1] == NULL)
            {
                printf("ERROR: No filename specified\n");
                continue;
            }

            undel(token[1]);
        }

        if (strcmp(token[0], "undelete") == 0)
        {
            if (imageIsOpen == 0)
            {
                printf("ERROR: Disk image is not open\n");
                continue;
            }

            if(token[1] == NULL)
            {
                printf("ERROR: No filename specified\n");
                continue;
            }

            undel(token[1]);
        }

        if (strcmp(token[0], "quit") == 0)
        {
            exit(0);
        }


        // Cleanup allocated memory
        for(int i = 0; i < MAX_NUM_ARGS; i++)
        {
            if(token[i] != NULL)
            {
                free(token[i]);
            }
        }
        free(headPtr);

        strcpy(commandString, "");
    }
    
    // Cleanup allocated memory
    free(commandString);


    return 0;
}