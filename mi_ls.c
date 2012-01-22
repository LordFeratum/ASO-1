/*
 * mi_ls.c
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
#include "directorios.h"

int main(int argc, char **argv){

	if(argc!=3){
		printf("Número de argumentos incorrecto \n");
		return -1;
	}

	char buffer[2048];
	memset(buffer,'\0',2048);

	bmount(argv[1]);	//Monta disco

	mi_dir(argv[2], buffer);

	printf("%s \n",buffer);

	bumount();	//Desmonta disco

	return 0;
}
