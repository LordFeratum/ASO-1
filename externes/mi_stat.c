/*
 * mi_stat.c
 *
 *  Created on: 16/01/2012
 *      Author: manuel
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include "directorios.h"


int main(int argc, char **argv){

	struct tm *tm; //ver info: struct tm
	struct STAT p_stat;

	bmount(argv[1]);

	mi_stat(argv[2], &p_stat);

	printf("Tipo: %c \n",p_stat.tipo);
	printf("Permisos: %c \n",p_stat.permisos);
	printf("Número de links: %d \n",p_stat.nlinks);
	printf("Tamaño: %d",p_stat.tamEnBytesLog);
	printf("Bloques ocupados: %d",p_stat.numBloquesOcupados);
	printf("Hora último acceso: %c",p_stat.tipo);

	tm = localtime(&p_stat.atime); // ver info: localtime()
	printf("Últimos acceso a datos: %d-%02d-%02d %02d:%02d:%02d\t",tm->tm_year+1900,
	tm->tm_mon+1,tm->tm_mday,tm->tm_hour,tm->tm_min,tm->tm_sec);

	tm = localtime(&p_stat.ctime); // ver info: localtime()
	printf("Última modificación de inodo: %d-%02d-%02d %02d:%02d:%02d\t",tm->tm_year+1900,
	tm->tm_mon+1,tm->tm_mday,tm->tm_hour,tm->tm_min,tm->tm_sec);

	tm = localtime(&p_stat.mtime); // ver info: localtime()
	printf("Última modificación de datos: %d-%02d-%02d %02d:%02d:%02d\t",tm->tm_year+1900,
	tm->tm_mon+1,tm->tm_mday,tm->tm_hour,tm->tm_min,tm->tm_sec);


	bumount();

	return 0;
}
