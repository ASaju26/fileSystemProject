CC=gcc

GCCFLAGS=-Wall -Werror -std=c99 -g

CINCS=-I./include

mfs: 
	gcc src/mfs.c src/createfs_savefs.c src/openfs_closefs.c src/insert.c src/encryptDecrypt.c \
	src/readFile.c src/retrieve.c src/delete.c src/attrib.c ${GCCFLAGS} ${CINCS} -o mfs


clean:
	rm -f ./mfs