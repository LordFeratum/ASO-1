/*
 * mi_cat.c
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

	unsigned int tam2,n;
	unsigned char buf[blocksize];
	FILE *salida;
	struct STAT p_stat;

	memset(buf,'\0',blocksize);

	if(argc!=3){	//Comprueba el número de argumentos
		printf("Número de argumentos incorrecto \n");
		return -1;
	}

	tam2 = strlen(argv[2]);
	if(argv[2][tam2-1]=='/'){	//Comprueba si es un fichero
		printf("No es un fichero \n");
		return -2;
	}

	bmount(argv[1]);	//Monta el disco
	mi_stat(argv[2], &p_stat);	//Extrae el tamaño en bytes para recorrer los bloques

	for(n=0;(n*blocksize)<p_stat.tamEnBytesLog;n++){	//Recorre los bloques y los escribe en stdout(salida estándar)
		if(mi_read(argv[2], buf, (n*blocksize), blocksize)<0){
			printf("Error al leer del fichero \n");
			return -1;
		}else{
			salida=fopen("/dev/stdout", "w");
			fwrite (buf,1,blocksize,salida);
			fclose(salida);
		}
	}
	printf("\n");

	bumount();	//Desmonta disco

	return 0;
}

