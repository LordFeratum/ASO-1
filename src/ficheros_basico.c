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
	if (((ninodos*128)%block)==0){
		return (ninodos*128)/block;
	}else{
		return ((ninodos*128)/block)+1;
	}
}


int initSB(unsigned int nbloques, unsigned int ninodos){
	sp.posPrimerBloqueMB=1; //Posición del primer bloque del mapa de bits
	sp.posUltimoBloqueMB = tamMB(nbloques)+sp.posPrimerBloqueMB; //Posición del último bloque del mapa de bits
	sp.posPrimerBloqueAI = sp.posUltimoBloqueMB+1; //Posición del primer bloque del array de inodos
	sp.posUltimoBloqueAI = tamAI(ninodos)+sp.posUltimoBloqueMB; //Posición del último bloque del array de inodos
	sp.posPrimerBloqueDatos = sp.posUltimoBloqueAI+1; //Posición del primer bloque de datos
	sp.posUltimoBloqueDatos = nbloques; //Posición del último bloque de datos
	sp.posInodoRaiz = sp.posPrimerBloqueAI; //Posición del inodo del directorio raíz
	sp.posPrimerInodoLibre = sp.posPrimerBloqueAI; //Posición del primer inodo libre
	sp.cantBloquesLibres = nbloques-sp.posPrimerBloqueDatos;  //Cantidad de bloques libres
	sp.cantInodosLibres = sp.posUltimoBloqueAI - sp.posPrimerBloqueAI; //Cantidad de inodos libres
	sp.totBloques = 0; //Cantidad total de bloques
	sp.totInodos = 0; //Cantidad total de inodos
	sp.padding[blocksize-12*sizeof(unsigned int)]; //Relleno

	//Falta escribir el superbloque
}

int initMB(unsigned int nbloques){
	int tMB = tamMB(nbloques);
	unsigned char buf[blocksize];

	memset(buf,0,blocksize);

	int i;

	for (i=1; i<tMB; i++){
		bwrite(i,buf);
	}
}
int initAI(unsigned int ninodos){
	//De superbloque sacar posicion inodo inicial
	int min=0;
	//De superbloque sacar numero mapabits

	for(int i=min;i<=max;i++){
		ino.tipo="l";
		ino.punterosDirectos[0]=; //12 punteros a bloques directos
	}

}
