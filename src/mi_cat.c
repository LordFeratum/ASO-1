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

	FILE *salida;
	struct STAT p_stat;
	int n;
	unsigned char buf[blocksize];

	bmount(argv[1]);

	mi_stat(argv[2], &p_stat);
	memset(buf, '\0', blocksize);

	for (n=0; (n*blocksize) < p_stat.tamEnBytesLog; n++){
		if(mi_read(argv[2], buf, n*blocksize, blocksize)){
			salida=fopen("dev/stdout", "w");
			fwrite (buf,1,blocksize,salida);
			fclose(salida);
		}
		memset(buf,'\0',blocksize);
	}

	bumount();

	return 0;
}

