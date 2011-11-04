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
#include "bloques.h"
#include "ficheros_basico.h"


int main(int argc, char **argv){

	unsigned int n_bloques = atoi(argv[2]);

	unsigned char buf[blocksize];
	struct superbloque sb;

	memset(buf,0,blocksize);

	bmount(argv[1]);
	int i;

	for (i=0; i<n_bloques; i++){
		bwrite(i,buf);
	}

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
	int c = sizeof(sb);
	printf("%d",c);


	bumount();
}
