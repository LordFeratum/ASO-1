/*
 * ficheros_basico.h
 *
 *  Created on: 11/10/2011
 *      Author: manuel
 */

// fichero ficheros_basico.h

#include "ficheros_basico.h"

int tamMB (unsigned int nbloques){
	if (nbloques % (blocksize*8)==0){
		return nbloques/(blocksize*8);
	}else{
		return (nbloques/(blocksize*8))+1;
	}
}


int tamAI (unsigned int ninodos){
	int block;
	block = blocksize/4;
	if (((ninodos*128)%block)==0){
		return (ninodos*128)/block;
	}else{
		return ((ninodos*128)/block)+1;
	}
}


int initSB(unsigned int nbloques, unsigned int ninodos);
int initMB(unsigned int nbloques);
int initAI(unsigned int ninodos);
