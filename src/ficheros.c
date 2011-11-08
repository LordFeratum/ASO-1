/*
 * ficheros.c
 *
 *  Created on: 11/10/2011
 *      Author: manuel
 */

// fichero ficherosc

#include "ficheros.h"

int mi_write_f(unsigned int inodo, const void *buf_original, unsigned int offset, unsigned int nbytes){

	unsigned char buf_bloque[blocksize];
	int fbloc,pbloc,dbloc,rbloc,i;
	unsigned int bfisico;
	struct inodo in;
	in = leer_inodo(inodo);

	if ((in.permisos & 2) != 2){
		return -1;
	}else{
		pbloc = offset / blocksize;
		dbloc = (offset + nbytes - 1) / blocksize;

		//primer bloc
		if (traducir_bloque_inodo(inodo,pbloc,&bfisico,1)){ //Mirar bfisico
			bread(bfisico,buf_bloque);
			rbloc = offset % blocksize;
			memcpy(buf_bloque + rbloc,buf_original, blocksize - rbloc);
			//bloc = traducir_bloque_inodo(inodo,bloc,in.punterosDirectos[pbloc],1);
			bwrite (bfisico,buf_bloque); //traducir bloque de nuevo?
		}else{
			return -1;
		}


		//blocs intermitjos
		for (i = (pbloc + 1); i < dbloc; i++){
			memcpy(buf_bloque,buf_original + (blocksize - rbloc) + (i - pbloc - 1) * blocksize,blocksize);
			if (traducir_bloque_inodo(inodo,i, &bfisico, 1)==0){
				bwrite(bfisico,buf_bloque);
			}else{
				return -1;
			}
		}

		//darrer bloc
		if (traducir_bloque_inodo(inodo,dbloc,&bfisico,1)){ //Mirar bfisico
			bread(bfisico,buf_bloque);
			fbloc = dbloc/blocksize;
			memcpy(buf_bloque,buf_original+ (blocksize - rbloc) + (dbloc - pbloc - 1) * blocksize, fbloc+1);
			//bloc = traducir_bloque_inodo(inodo,bloc,in.punterosDirectos[pbloc],1);
			bwrite (bfisico,buf_bloque); //traducir bloque de nuevo?
		}
	return nbytes;
	}
}
