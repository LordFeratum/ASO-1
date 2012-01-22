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

	if(argc!=4){
		printf("Número de argumentos incorrecto \n");
		return -1;
	}

	int tam;

	bmount(argv[1]);	//Monta disco

	unsigned int modo = atoi(argv[3]);

	tam = strlen(argv[2]);

	if(argv[2][tam-1]!='/'){
		if(mi_creat(argv[2], modo)<0){
			printf("Error en la creación de archivo \n");
		}else{
			printf("Archivo creado \n");
		}
	}else{
		printf("Debe introducir un fichero válido \n");
	}

	bumount();	//Desmonta disco

	return 0;
}

