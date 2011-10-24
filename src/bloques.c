/*
 ============================================================================
 Name        : Fichero.c
 Author      : Manuel Pol
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include "bloques.h"

static int fichero = 0;

int bmount (const char *camino){
	fichero = open(camino, O_RDWR|O_CREAT,0666);
	if(fichero == -1){
		printf("Error de apertura de archivo ");
	}
	return fichero;
}

int bumount(){
	if(close(fichero) < 0)
		printf("Error al cerrar de archivo");
		return 1;
	return 0;
}

int bwrite(unsigned int bloque, const void *buf){
	if (lseek(fichero,(bloque*blocksize),SEEK_SET) == -1){
		printf("Error al ubicar el archivo");
		return -1;
	}
    if (write(fichero, buf, blocksize) == -1) {
        printf("Error al escribir en el archivo");
        return -1;
	}
	return 0;
}

int bread(unsigned int bloque, void *buf){
	lseek(fichero,bloque *blocksize,SEEK_SET);
	if (read(0,buf,bloque)== -1){
		printf("Error al leer del archivo");
	}
}


