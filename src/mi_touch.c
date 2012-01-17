/*
 * mi_touch.c
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

	if(argv!=4){
		return -1;
		printf("Número de argumentos incorrecto");
	}

	int tam;

	bmount(argv[1]);

	unsigned int modo = atoi(argv[2]);

	tam = strlen(argv[3]);

	if(!argv[3][tam-1]=='/'){
		if(mi_creat(argv[3], modo)<0){
			printf("Error en la creación de archivo \n");
		}else{
			printf("Archivo creado \n");
		}
	}else{
		printf("Debe introducir un fichero válido \n");
	}

	bumount();

	return 0;
}

