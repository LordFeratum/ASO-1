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
	int sem;
	iniciarSem(&sem);

	unsigned int n_bloques = atoi(argv[2]);

	unsigned char buf[blocksize];
	struct superbloque sb;

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



	reservar_inodo('d',7);
	reservar_inodo('d',7);
	reservar_inodo('d',7);

	int c = sizeof(sb);
	printf("%d",c);

	//escribir_bit(34,1);
	//escribir_bit(35,1);
	//escribir_bit(37,1);
	int d = leer_bit(33);

	printf("el bit es %d \n",d);

	printf("Bloque reservado: %d \n", e);

	leer_bit(e);

	printf("el bit del bloque reservado es %d \n",d);

	int f = reservar_bloque();

	printf("Bloque reservado: %d \n", f);

	f = reservar_bloque();

	printf("Bloque reservado: %d \n", f);

	f = reservar_bloque();

	printf("Bloque reservado: %d \n", f);

	f = reservar_bloque();

	printf("Bloque reservado: %d \n", f);

	f = reservar_bloque();

	printf("Bloque reservado: %d \n", f);

	liberar_bloque(1032);

	struct inodo ino = leer_inodo(25);

	printf("Punteros directos %d \n" ,ino.punterosDirectos[0]);
	printf("Tipo: %c \n", ino.tipo);

	ino.punterosDirectos[0]=6;
	ino.tipo='f';
	ino.permisos='7';

	escribir_inodo(ino,25);
	ino = leer_inodo(0);
	struct inodo ino2 = leer_inodo(1);
	struct inodo ino3 = leer_inodo(2);

	printf("Tipo: %c \n", ino.tipo);
	printf("Tipo: %c \n", ino2.tipo);
	printf("Tipo: %c \n", ino3.tipo);

	liberar_inodo(1);
	ino2 = leer_inodo(1);
	printf("Tipo: %c \n", ino2.tipo);


	unsigned char elbufS[blocksize];
	memset(elbufS,254,blocksize);

	mi_write_f(1,elbufS,0,blocksize);


	unsigned char elbufR[blocksize];

	struct inodo superino = leer_inodo(2);

	printf("Bloques usados: %d \n", superino.numBloquesOcupados);

	bread(posSB,&sb);

	memset(elbufR,0,blocksize);

	mi_read_f(2,elbufR,0,blocksize);

	printf("Hola");













	bumount();
	return 0;
}
