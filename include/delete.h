#ifndef _DELETE_H
#define _DELETE_H


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stddef.h>
#include <ctype.h>
#include <getopt.h>
#include <string.h>

#include "mfs.h"

void delete(char *deleteFileName);

void undel(char *deleteFilenName);

#endif