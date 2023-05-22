#include "openfs_closefs.h"

void openfs(char* filename)
{
    fp = fopen(filename, "r");

    if (fp == NULL)
    {
        printf("ERROR: Cannot open file\n");
        return;
    }

    strncpy(imageName, filename, strlen(filename));

    fread(&data[0][0], BLOCK_SIZE, NUM_BLOCKS, fp);

    imageIsOpen = 1;

    //fclose(fp); // error, remove this
}

void closefs()
{
    if (imageIsOpen == 0)
    {
        printf("ERROR: Disk image is not open\n");
        return;
    }

    fclose(fp);

    imageIsOpen = 0;

    memset(imageName, 0, 64);
}