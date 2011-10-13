/*
 * ficheros_basico.h
 *
 *  Created on: 11/10/2011
 *      Author: manuel
 */

// fichero ficheros_basico.h

#include <time.h>
#include "bloques.h"

#define posSB 0 //el superbloque se escribe en el primer bloque de nuestro FS
#define T_INODO 128 //tamaño en bytes de un inodo

struct superbloque{
	unsigned int posPrimerBloqueMB; //Posición del primer bloque del mapa de bits
	unsigned int posUltimoBloqueMB; //Posición del último bloque del mapa de bits
	unsigned int posPrimerBloqueAI; //Posición del primer bloque del array de inodos
	unsigned int posUltimoBloqueAI; //Posición del último bloque del array de inodos
	unsigned int posPrimerBloqueDatos; //Posición del primer bloque de datos
	unsigned int posUltimoBloqueDatos; //Posición del último bloque de datos
	unsigned int posInodoRaiz; //Posición del inodo del directorio raíz
	unsigned int posPrimerInodoLibre; //Posición del primer inodo libre
	unsigned int cantBloquesLibres; //Cantidad de bloques libres
	unsigned int cantInodosLibres; //Cantidad de inodos libres
	unsigned int totBloques; //Cantidad total de bloques
	unsigned int totInodos; //Cantidad total de inodos
	char padding[blocksize-12*sizeof(unsigned int)]; //Relleno
};

struct inodo{
	unsigned char tipo; //Tipo (libre, directorio o fichero)
	unsigned char permisos; //Permisos (lectura y/o escritura y/o ejecución)
	time_t atime; //Fecha y hora del último acceso a datos: atime
	time_t mtime; //Fecha y hora de la última modificación de datos: mtime
	time_t ctime; //Fecha y hora de la última modificación del inodo: ctime
	unsigned int nlinks; //Cantidad de enlaces de entradas en directorio
	unsigned int tamEnBytesLog; //Tamaño en bytes lógicos
	unsigned int numBloquesOcupados; //Cantidad de bloques ocupados en la zona de datos
	unsigned int punterosDirectos[12]; //12 punteros a bloques directos
	unsigned int punterosIndirectos[3]; //3 punteros a bloques indirectos:1 puntero indirecto simple, 1 puntero indirecto doble, 1 puntero indirecto triple
	char padding[T_INODO-(2*sizeof(char))-(18*sizeof(int))-(3*sizeof(time_t))];
};

int tamMB (unsigned int nbloques);
int tamAI (unsigned int ninodos);
int initSB(unsigned int nbloques, unsigned int ninodos);
int initMB(unsigned int nbloques);
int initAI(unsigned int ninodos);