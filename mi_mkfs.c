/*
 * mi_mkfs.c
 *
 *  Created on: 04/10/2011
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

	if(argc!=3){
		printf("Número de argumentos incorrecto \n");
		return -1;
	}

	unsigned int n_bloques = atoi(argv[2]);	//Número de bloques del FS

	unsigned char buf[blocksize];

	memset(buf,0,blocksize);

	bmount(argv[1]);
	int i;

	for (i=0; i<n_bloques-1; i++){
		bwrite(i,buf);
	}
	memset(buf,1,blocksize);

	bwrite(n_bloques,buf);


	int block = n_bloques/4;//Para el cáculo de inodos
	if(initSB(n_bloques, block)==-1){
		printf("Error al escribir superbloque");
	}else{
		printf("Superbloque escrito \n");
	}

	if(initMB(n_bloques)==-1){
		printf("Error al escribir mapa de bits");
	}else{
		printf("Mapa de bits escrito \n");
	}

	int n = tamAI(block);//En teoría no sirve para nada

	if (initAI(n)==-1){
		printf("Error al escribir el array de inodes");
	}else{
		printf("Array de inodes escrito \n");
	}
	int inodoraiz = reservar_inodo('d',7);

	printf("Sistema de ficheros creado correctamente \n");	
	
	bumount();

	return 0;
}
