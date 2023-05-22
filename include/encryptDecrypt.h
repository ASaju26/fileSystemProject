#ifndef _ENCRYPTDECRYPT_H
#define _ENCRYPTDECRYPT_H


#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "mfs.h"
#define BLOCK_SIZE 1024
#define NUM_BLOCKS 65536
void encryptDecrypt(char *filename, char key);

unsigned char *file_data;

#endif