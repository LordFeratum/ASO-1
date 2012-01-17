/*
 * mi_chmod.c
 *
 *  Created on: 20/12/2011
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

	bmount(argv[1]);

	unsigned int modo = atoi(argv[2]);

	if(mi_chmod(argv[3], modo)<0){
		printf("Error en el cambio de permisos \n");
	}else{
		printf("Permisos cambiados \n");
	}

	bumount();

	return 0;
}
