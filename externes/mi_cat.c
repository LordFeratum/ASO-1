/*
 * mi_cat.c
 *
 *  Created on: 16/01/2012
 *      Author: manuel
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include "directorios.h"


int main(int argc, char **argv){

	FILE *file;

	bmount(argv[1]);

	unsigned char buff[blocksize];
	memset(buff, '\0', blocksize);

	bumount();

	return 0;
}

