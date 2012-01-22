/*
 * mi_ln.c
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

	if(argc!=4){
		printf("Número de argumentos incorrecto \n");
		return -1;
	}

	bmount(argv[1]);	//Monta disco

	if(mi_link(argv[2], argv[3])<0){
		printf("Error al enlazar el fichero o directorio \n");
	}else{
		printf("Enlace realizado correctamente \n");
	}

	bumount();	//Desnonta disco

	return 0;
}
