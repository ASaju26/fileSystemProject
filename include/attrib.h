#ifndef _ATTRIB_H
#define _ATTRIB_H


#include "mfs.h"

#define HIDDEN 0x1
#define READONLY 0x2

void attrib(char* filename, char* change);

#endif