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


int main(int argc, char **argv){
	char nombre[5]= "hola";
	printf("Introducido %s", argv[1]);

	bmount("fichero");
}
