/*
 * mi_rm.c
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

	if(argc!=3){
		printf("Número de argumentos incorrecto \n");
		return -1;
	}

	bmount(argv[1]);	//Monta disco

	if(mi_unlink(argv[2])<0){
		printf("Error al borrar el fichero/directorio \n");
	}else{
		printf("Borrado correctamente \n");
	}

	bumount();	//Desmonta disco

	return 0;
}



