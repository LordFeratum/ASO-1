/*
 * mi_mkfs.c
 *
 *  Created on: 04/10/2011
 *      Author: manuel
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "bloques.h"


int main(int argc, char **argv){

	unsigned int n_bloques = atoi(argv[2]);

	unsigned char buf[blocksize];

	memset(buf,0,blocksize);

	bmount(argv[1]);
	int i;

	for (i=0; i<n_bloques; i++){
		bwrite(i,buf);
	}
	bumount();
}
