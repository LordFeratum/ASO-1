
/*
 * ficheros.h
 *
 *  Created on: 8/11/2011
 *      Author: Joan Ll.
 */

// fichero ficheros.h

#include <time.h>
#include "bloques.h"
#include "ficheros_basico.h"

int mi_write_f(unsigned int inodo, const void *buf_original, unsigned int offset, unsigned int nbytes);
int mi_read_f(unsigned int inodo, void *buf_original, unsigned int offset, unsigned int nbytes);
