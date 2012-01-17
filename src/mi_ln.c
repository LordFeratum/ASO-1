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

	if(argv!=4){
		return -1;
		printf("Número de argumentos incorrecto");
	}

	bmount(argv[1]);

	if(mi_link(argv[2], argv[3])<0){
		printf("Error al enlazar el fichero o directorio \n");
	}else{
		printf("Enlace realizado correctamente \n");
	}

	bumount();

	return 0;
}
