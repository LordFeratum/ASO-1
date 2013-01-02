/*
 * mi_read.c
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

	unsigned int tam2,tam3,tam4;
	unsigned char buf[blocksize];

	memset(buf,'\0',blocksize);

	if(argc!=5){
		printf("Número de argumentos incorrecto \n");
		return -1;
	}

	tam2 = strlen(argv[2]);
	tam3 = atoi(argv[4]);
	tam4 = atoi(argv[3]);
	
	if(argv[2][tam2-1]=='/'){
		printf("No es un fichero \n");
		return -2;
	}

	bmount(argv[1]);	//Monta disco

	if(mi_read(argv[2], buf, tam4, tam3)<0){
		printf("Error al leer del fichero \n");
	}else{
		printf("%s \n",buf);
	}

	bumount();	//Desmonta disco

	return 0;
}
