/*
 * mi_write.c
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

	int tam2,tam3,tam4;

	if(argc!=5){
		printf("Número de argumentos incorrecto \n");
		return -1;
	}

	tam2 = strlen(argv[2]);
	tam3 = strlen(argv[3]);
	tam4 = atoi(argv[4]);
	if(argv[2][tam2-1]=='/'){
		printf("No es un fichero \n");
		return -2;
	}

	bmount(argv[1]);	//Monta disco

	if(mi_write(argv[2], argv[3], tam4, tam3)<0){
		printf("Error al escribir en el fichero \n");
	}else{
		printf("Fichero escrito correctamente \n");
	}

	bumount();	//Desmonta disco

	return 0;
}
