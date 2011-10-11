/*
 ============================================================================
 Name        : Fichero.c
 Author      : Manuel Pol
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */


#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> /* Modos de apertura y función open()*/
#include <stdlib.h> /* Funciones write() y close() */
#include <string.h>

#define blocksize 1024 //bytes

int bmount(const char *camino);
int bumount();
int bwrite(unsigned int bloque, const void *buf);
int bread(unsigned int bloque, void *buf);
