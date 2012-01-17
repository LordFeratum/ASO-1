/*
 * mi_ls.c
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
#include "directorios.h"

int main(int argc, char **argv){

	char buffer[500];

	bmount(argv[1]);

	mi_dir(argv[2], buffer);

	printf("%s",buffer);

	bumount();

	return 0;
}
