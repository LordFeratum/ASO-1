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

	if ((in.permisos & 2) != 2){	//Si no tiene permisos de escritura
		return -1;
	}else{
		pbloc = offset / blocksize;	//Primer bloque
		dbloc = (offset + nbytes - 1) / blocksize;	//Último bloque

		//primer bloc
		if (traducir_bloque_inodo(inodo,pbloc,&bfisico,'1')>-1){ //Si existe escribe
			bread(bfisico,buf_bloque);
			rbloc = offset % blocksize;
			if(nbytes<=(blocksize-offset)){	//Si cabe en un bloque
				memcpy(buf_bloque + rbloc,buf_original, nbytes);
			}else{	//Si no cabe
				memcpy(buf_bloque + rbloc,buf_original, blocksize - rbloc);
			}
			bwrite (bfisico,buf_bloque);	//Escribe el bloque
		}else{
			return -1;
		}


		//Recorre bloques intermedios
		for (i = (pbloc + 1); i < dbloc; i++){
			memcpy(buf_bloque,buf_original + (blocksize - rbloc) + (i - pbloc - 1) * blocksize,blocksize);
			if (traducir_bloque_inodo(inodo,i, &bfisico, '1')==0){	//Copia bloques completos
				printf("Bloque escrito: %d \n",bfisico);
				bwrite(bfisico,buf_bloque);
			}else{
				return -1;
			}
		}

		//Último bloque
		if (pbloc<dbloc){
			if (traducir_bloque_inodo(inodo,dbloc,&bfisico,'1')){ //Si existe escribe desde el principio del bloque
				bread(bfisico,buf_bloque);
				fbloc = (offset + nbytes - 1)/blocksize;
				memcpy(buf_bloque,buf_original+ (blocksize - rbloc) + (dbloc - pbloc - 1) * blocksize, fbloc+1);
				bwrite (bfisico,buf_bloque);
			}
		}

		in = leer_inodo(inodo);
		if (in.tamEnBytesLog<offset+nbytes){	//Si es más grande modifica el tamaño del fichero
			in.tamEnBytesLog=offset+nbytes;
		}
		int numbloq = in.numBloquesOcupados;
		in.numBloquesOcupados=numbloq+(dbloc-pbloc);
		in.ctime=time(NULL);
		in.atime=time(NULL);
		in.mtime=time(NULL);
		escribir_inodo(in,inodo);	//Escribe inodo modificado
		return nbytes;
	}
}

int mi_read_f(unsigned int inodo, void *buf_original, unsigned int offset, unsigned int nbytes){
	unsigned char buf_bloque[blocksize];
	unsigned int bfisico;
	int pbloc,dbloc,rbloc,i,fbloc;
	struct inodo in;
	in = leer_inodo(inodo);
	memset(buf_bloque,0,blocksize);
	if ((in.permisos & 4)!=4){	//Si tiene permisos de lectura
		return -1;
	}else{
		pbloc = offset / blocksize;	//Primer bloque
		dbloc = (offset + nbytes - 1) / blocksize;	//Último bloque

		if (traducir_bloque_inodo(inodo,pbloc,&bfisico,'0')>=0){	//Si existe lee
			bread(bfisico,&buf_bloque);
			rbloc = offset % blocksize;
			if(nbytes<=(blocksize-offset)){	//Si cabe en un bloque
				memcpy(buf_original,buf_bloque + rbloc, nbytes);
			}else{	//Si no cabe
				memcpy(buf_original,buf_bloque + rbloc, blocksize - rbloc);
			}
		}else{
			return -1;
		}
		//Recorre bloques intermedios
		for (i = (pbloc + 1); i < dbloc; i++){
			if (traducir_bloque_inodo(inodo,i, &bfisico, '0')==0){	//Lee bloques completos
				bread(bfisico,buf_bloque);
				memcpy(buf_original + (blocksize - rbloc) + (i - pbloc - 1) * blocksize,&buf_bloque,blocksize);
			}else{
				return -1;
			}
		}
		//Último bloque
		if (pbloc<dbloc){
					if (traducir_bloque_inodo(inodo,dbloc,&bfisico,'0')){ //Si existe escribe desde el principio del bloque
						bread(bfisico,buf_bloque);
						fbloc = (offset + nbytes - 1)%blocksize;
						memcpy(buf_original+ (blocksize - rbloc) + (dbloc - pbloc - 1) * blocksize,&buf_bloque, fbloc+1);
					}
		}
		in = leer_inodo(inodo);
		in.atime=time(NULL);
		escribir_inodo(in,inodo);	//Escribe inodo modificado
		return nbytes;
	}
}
//Cambia permisos de un inodo
int mi_chmod_f(unsigned int ninodo, unsigned char modo){
	struct inodo in;
	in = leer_inodo(ninodo);
	in.permisos=modo;
	in.ctime=time(NULL);
	escribir_inodo(in,ninodo);
	return 0;
}
//Borra desde nbytes hasta el final del fichero
int mi_truncar_f(unsigned int ninodo, unsigned int nbytes){
	int pbloc,dbloc,i;
	struct inodo in;

	in = leer_inodo(ninodo);
	if ((in.permisos & 2) != 2){	//Si tiene permisos de escritura
			return -1;
	}else{
		pbloc = nbytes / blocksize;	//Primer bloque
		dbloc=in.tamEnBytesLog/blocksize;	//Último bloque

		for (i=pbloc+1;i<=dbloc;i++){	//Libera bloques
			liberar_bloques_inodo(ninodo, i);
		}

		in.tamEnBytesLog=nbytes;
		in.ctime=time(NULL);
		in.mtime=time(NULL);
		escribir_inodo(in,ninodo);	//Actualiza inodo
		return 0;
	}
}
//Lee los datos de un inodo
int mi_stat_f(unsigned int ninodo, struct STAT *p_stat){
	struct inodo in;

	in = leer_inodo(ninodo);

	p_stat ->tipo = in.tipo;
	p_stat ->permisos = in.permisos;
	p_stat ->numBloquesOcupados = in.numBloquesOcupados;
	p_stat ->tamEnBytesLog = in.tamEnBytesLog;
	p_stat ->nlinks = in.nlinks;
	p_stat ->atime = in.atime;
	p_stat ->mtime = in.mtime;
	p_stat ->ctime = in.ctime;

	return 0;
}


