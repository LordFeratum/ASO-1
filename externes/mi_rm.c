/*
 * mi_rm.c
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

	bmount(argv[1]);

	mi_unlink(argv[2]);

	bumount();

	return 0;
}



