/*
 * mi_mkdir.c
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

	if(argc!=4){
		printf("Número de argumentos incorrecto \n");
		return -1;
	}

	int tam;

	bmount(argv[1]);

	unsigned int modo = atoi(argv[3]);

	tam = strlen(argv[2]);

	if(argv[2][tam-1]=='/'){
		if(mi_creat(argv[2], modo)<0){
			printf("Error al crear directorio \n");
		}else{
			printf("Directorio creado correctamente \n");
		}
	}else{
		printf("Debe introducir un directorio válido \n");
	}

	bumount();

	return 0;
}


