/*
 * directorios.h
 *
 *  Created on: 22/11/2011
 *      Author: manuel
 */

#include "bloques.h"
#include "ficheros_basico.h"
#include "ficheros.h"
#include <string.h>

struct entrada{
	char nombre[60];
	unsigned int inodo;
};


int extraer_camino(const char *camino, char *inicial, char *final);
int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir, unsigned int *p_inodo, unsigned int *p_entrada, char reservar, unsigned char modo);
int mi_creat(const char *camino, unsigned char modo);
int mi_link(const char *camino1, const char *camino2);
int mi_unlink(const char *camino);


