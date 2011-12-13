/*
 * directorios.c
 *
 *  Created on: 22/11/2011
 *      Author: manuel
 */
#include "directorios.h"

int extraer_camino(const char *camino, char *inicial, char *final){
	int tam,n,r;
	if (camino[0]!='/'){
		return -1;
	}
	while (camino[n]!='/'){
		inicial[n-1] = camino[n];
		n++;
	}
	if (tam == n){
		final='\0';
		return 0; //fichero
	}
	while (n<tam){
		final[r] = camino[n];
		n++;
		r++;
	}
	return 1; //directorio
}

int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir, unsigned int *p_inodo, unsigned int *p_entrada, char reservar, unsigned char modo){
	char inicial[60],final[60];
	struct inodo in;
	struct entrada ent;
	int numentradas,nentrada,tipo;
	if (strcmp(camino_parcial,"/")==0){
		*p_inodo=0;
		*p_entrada=0;
		return 0;
	}

	memset(inicial,'\0',60);
	memset(final,'\0',60);

	tipo = extraer_camino(camino_parcial,inicial,final);
	if (tipo==-1){
		return -1;
	}
	in = leer_inodo(*p_inodo_dir);
	memset(ent.nombre,'\0',60);
	numentradas = in.tamEnBytesLog/sizeof(struct entrada);
	nentrada = 0;
	if(numentradas>0){
		if(mi_read_f(*p_inodo_dir,&ent,0,sizeof(struct entrada)) == -1){
			return -2; //No permisos de lectura
		}
		while ((nentrada<numentradas) && (strcmp(inicial,ent.nombre))){
			nentrada++;
			if(mi_read_f(*p_inodo_dir,&ent,nentrada*sizeof(struct entrada),sizeof(struct entrada))==-1){
				return -2;
			}
		}
		if((nentrada<numentradas) && (reservar==1)){
			return -5; //Modo escritura y ya existe
		}

	}
	if(nentrada==numentradas){
		switch (reservar){
		case '0':
			return -4; //Modo consulta, no existeix
			break;
		case '1':
			strcpy(ent.nombre,inicial);
			if(tipo==1){
				if(strcmp(final,"/")){
					ent.inodo = reservar_inodo('d',modo); //char?? WTF!
				}else{
					ent.inodo = reservar_inodo('d',7);
				}
			}else{
				ent.inodo = reservar_inodo('f',modo);
			}
			if(mi_write_f(*p_inodo_dir,&ent,nentrada*sizeof(struct entrada),sizeof(struct entrada))==-1){
				if(ent.inodo!=-1){
					liberar_inodo(ent.inodo);
				}
				return -3; //El directorio donde apunta p_inodo_dir no tiene permisos de escritura
			}
			break;
		}
	}
	if(strcmp(final,"/") == 0 || strcmp(final,"") == 0){
		*p_inodo = ent.inodo;
		*p_entrada = nentrada; //Comprovar nentrada
		return 0;
	}else{
		*p_inodo_dir = ent.inodo; //Comprobar ent.inodo
		buscar_entrada(final,p_inodo_dir,p_inodo,p_entrada,reservar,modo);
	}
	return 0;
}

int mi_creat(const char *camino, unsigned char modo){
	unsigned int p_inodo_dir=0;
	unsigned int p_inodo= 0;
	unsigned int p_entrada = 0;
	int bEnt;
	bEnt= buscar_entrada(camino, &p_inodo_dir, &p_inodo,&p_entrada,'1',modo); //punteros?
	if(bEnt<0){
		switch(bEnt){
		case -1:
			printf("Tipo no adecuado");
			break;
		case -2:
			printf("No se puede leer el inodo");
			break;
		case -3:
			printf("El directorio donde apunta p_inodo_dir no tiene permisos de escritura");
			break;
		case -5:
			printf("Ya existe");
			break;
		}
		return -1;
	}
	return 0; //No hemos hecho nada con el nombre
}

int mi_link(const char *camino1, const char *camino2){
	unsigned int p_inodo_dir = 0;
	unsigned int p_inodo = 0;
	unsigned int p_entrada = 0;
	struct entrada entrada;
	struct inodo inodo;

	buscar_entrada(camino1,&p_inodo_dir,&p_inodo,&p_entrada,'0','0');
	int ninodo = p_inodo;

	p_inodo_dir = 0;
	p_inodo = 0;
	p_entrada = 0;

	int bEnt = buscar_entrada(camino2,&p_inodo_dir,&p_inodo,&p_entrada,'1','0');

	if(bEnt<0){
		switch(bEnt){
		case -1:
			printf("Tipo no adecuado");
			break;
		case -2:
			printf("No se puede leer el inodo");
			break;
		case -3:
			printf("El directorio donde apunta p_inodo_dir no tiene permisos de escritura");
			break;
		case -5:
			printf("Ya existe");
			break;
		}
		return -1;
	}else{
		if(mi_read_f(p_inodo_dir,&entrada,p_entrada*sizeof(struct entrada),sizeof(struct entrada))==-1){
			return -2;
		}
		liberar_inodo(entrada.inodo);
		entrada.inodo = ninodo;
		if(mi_write_f(p_inodo_dir,&entrada,p_entrada*sizeof(struct entrada),sizeof(struct entrada))==-1){
			return -3; //El directorio donde apunta p_inodo_dir no tiene permisos de escritura
		}else{
			inodo = leer_inodo(ninodo);
			inodo.nlinks++;
			inodo.ctime = time(NULL);
			escribir_inodo(inodo,ninodo);
		}
	}
	return 0;
}
