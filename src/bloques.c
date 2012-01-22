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
	int sem;
	iniciarSem(&sem);	//Inicia semáforo
	fichero = open(camino, O_RDWR|O_CREAT,0666); //Monta disco en R/W
	if(fichero == -1){
		printf("Error de apertura de archivo");
	}
	return fichero;
}

int bumount(){
	int sem;
	if(!sem){
		obtenerSem(&sem);
	}
	eliminarSem(sem);	//Finaliza semáforo
	if(close(fichero) < 0)	//Desmonta disco
		printf("Error al cerrar de archivo");
		return 1;
	return 0;
}

int bwrite(unsigned int bloque, const void *buf){
	if (lseek(fichero,(bloque*blocksize),SEEK_SET) == -1){	//Ubica bloque
		printf("Error al ubicar el archivo");
		return -1;
	}
    if (write(fichero, buf, blocksize) == -1) {	//Escribe el bloque
        printf("Error al escribir en el archivo");
        return -1;
	}
	return 0;
}

int bread(unsigned int bloque, void *buf){
	if (lseek(fichero,(bloque*blocksize),SEEK_SET) == -1){	//Ubica bloque
		printf("Error al ubicar la lectura del archivo");
		return -1;
	}
	if (read(fichero,buf,blocksize)== -1){	//Escribe el bloque
		printf("Error al leer del archivo");
		return -1;
	}
	return 0;
}


