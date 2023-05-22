#include "createfs_savefs.h"
#include "mfs.h"

void createfs(char* filename)
{
    fp = fopen(filename, "w");

    if (fp == NULL)
    {
        printf("ERROR: Cannot open file\n");
        return;
    }

    strncpy(imageName, filename, strlen(filename));

    memset(data, 0, NUM_BLOCKS * BLOCK_SIZE);

    init();

    imageIsOpen = 1;

    fclose(fp);
}

void savefs()
{
    if (imageIsOpen == 0)
    {
        printf("ERROR: Disk image is not open\n");
        return;
    }

    fp = fopen(imageName, "w");

    if (fp == NULL)
    {
        printf("ERROR: Cannot open file\n");
        return;
    }

    fwrite(&data[0][0], BLOCK_SIZE, NUM_BLOCKS, fp);

    memset(imageName, 0, 64);

    fclose(fp);
}
