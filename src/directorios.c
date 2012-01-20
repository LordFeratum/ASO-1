/*
 * directorios.c
 *
 *  Created on: 22/11/2011
 *      Author: manuel
 */
#include "directorios.h"
int sem = 0;

int extraer_camino(const char *camino, char *inicial, char *final){
	int tam,n,r;
	if (camino[0]!='/'){
		return -1;
	}
	n=1;
	while ((camino[n]!='/') && camino[n]!='\0'){
		inicial[n-1] = camino[n];
		n++;
	}
	tam = strlen(camino);
	if (tam == n){
		final='\0';
		return 0; //fichero
	}
	r=0;
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
	}
	if(nentrada==numentradas){
		switch (reservar){
		case '0':
			return -4; //Modo consulta, no existeix
			break;
		case '1':
			strcpy(ent.nombre,inicial);
			if(tipo==1){
				if(!strcmp(final,"/")){
					ent.inodo = reservar_inodo('d',modo);
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
		if((nentrada<numentradas) && (reservar=='1')){
			return -5; //Modo escritura y ya existe
		}
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
	if(!sem){
		obtenerSem(&sem);
	}
	waitSem(sem);
	bEnt= buscar_entrada(camino, &p_inodo_dir, &p_inodo,&p_entrada,'1',modo); //punteros?
	signalSem(sem);
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

	if(!sem){
		obtenerSem(&sem);
	}
	waitSem(sem);
	int rest = buscar_entrada(camino1,&p_inodo_dir,&p_inodo,&p_entrada,'0','0');
	signalSem(sem);

	if(rest<0){
		switch(rest){
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
	int ninodo = p_inodo;

	p_inodo_dir = 0;
	p_inodo = 0;
	p_entrada = 0;

	if(!sem){
		obtenerSem(&sem);
	}
	waitSem(sem);
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
		signalSem(sem);
		return -1;
	}else{
		if(mi_read_f(p_inodo_dir,&entrada,p_entrada*sizeof(struct entrada),sizeof(struct entrada))==-1){
			signalSem(sem);
			return -2;
		}
		liberar_inodo(entrada.inodo);
		entrada.inodo = ninodo;
		if(mi_write_f(p_inodo_dir,&entrada,p_entrada*sizeof(struct entrada),sizeof(struct entrada))==-1){
			signalSem(sem);
			return -3; //El directorio donde apunta p_inodo_dir no tiene permisos de escritura
		}else{
			inodo = leer_inodo(ninodo);
			inodo.nlinks++;
			inodo.ctime = time(NULL);
			escribir_inodo(inodo,ninodo);
		}
	}
	signalSem(sem);
	return 0;
}


int mi_unlink(const char *camino){
	unsigned int p_inodo_dir=0;
	unsigned int p_inodo= 0;
	unsigned int p_entrada = 0;
	struct entrada buf_original,buf_com;
	int nentradas;
	struct inodo inodo,inodoF;

	if(!sem){
		obtenerSem(&sem);
	}
	waitSem(sem);
	int bEnt = buscar_entrada(camino,&p_inodo_dir,&p_inodo,&p_entrada,'0','0');

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
		case -4:
			printf("No existe");
			break;
		}
		signalSem(sem);
		return -1;
	}else{
		inodo = leer_inodo(p_inodo_dir);
		nentradas = inodo.tamEnBytesLog/sizeof(struct entrada);
		mi_read_f(p_inodo_dir, &buf_original, p_entrada*sizeof(struct entrada), sizeof(struct entrada));
		inodoF = leer_inodo(buf_original.inodo);
		if((inodoF.tipo=='d') && (inodoF.tamEnBytesLog>=64)){
			printf("El directorio no está vacío \n");
			return -5;
		}
		if (nentradas-1==p_entrada){
			mi_truncar_f(p_inodo_dir,inodo.tamEnBytesLog-sizeof(struct entrada));
		}else{
			mi_read_f(p_inodo_dir, &buf_com, (nentradas-1)*sizeof(struct entrada), sizeof(struct entrada));
			mi_write_f(p_inodo_dir, &buf_com, p_entrada*sizeof(struct entrada), sizeof(struct entrada));
			mi_truncar_f(p_inodo_dir,inodo.tamEnBytesLog-sizeof(struct entrada));
		}
		inodoF.nlinks--;
		if (inodoF.nlinks==0){
			liberar_inodo(buf_original.inodo);
		}else{
			inodoF.ctime=time(NULL);
			escribir_inodo(inodoF,buf_original.inodo);
		}
	}
	signalSem(sem);
	return 0;
}

int mi_dir(const char *camino, char *buffer){
	unsigned int p_inodo_dir=0;
	unsigned int p_inodo= 0;
	unsigned int p_entrada = 0;
	int nentradas,i;
	struct inodo inodo;
	struct tm *tm; //ver info: struct tm
	char tmp[100];
	struct entrada ent;
	unsigned char relleno[32];

	int bEnto = buscar_entrada(camino,&p_inodo_dir,&p_inodo,&p_entrada,'0','0');

	if(bEnto<0){
		switch(bEnto){
		case -1:
			printf("Tipo no adecuado");
			break;
		case -2:
			printf("No se puede leer el inodo");
			break;
		case -3:
			printf("El directorio donde apunta p_inodo_dir no tiene permisos de escritura");
			break;
		case -4:
			printf("No existe");
			break;
		}
	}else{
		inodo = leer_inodo(p_inodo);
		if((inodo.tipo=='d') && (inodo.permisos & 4)){
			nentradas = inodo.tamEnBytesLog/sizeof(struct entrada);
			for (i=0; i<nentradas;i++){
				mi_read_f(p_inodo, &ent, i*sizeof(struct entrada), sizeof(struct entrada));
				inodo = leer_inodo(ent.inodo);

				sprintf(relleno,"%c",inodo.tipo);
				strcat(buffer,relleno);

				if (inodo.permisos & 4)
					strcat(buffer,"r");
				else
					strcat(buffer,"-");
				if (inodo.permisos & 2)
					strcat(buffer,"w");
				else
					strcat(buffer,"-");
				if (inodo.permisos & 1)
					strcat(buffer,"x");
				else
					strcat(buffer,"-");

				sprintf(relleno,"  %d  %d  %d  ",ent.inodo,inodo.nlinks,inodo.tamEnBytesLog);
				strcat(buffer,relleno);

				//Para incorporar la información acerca del tiempo:
				tm = localtime(&inodo.mtime); // ver info: localtime()
				sprintf(tmp,"%d-%02d-%02d %02d:%02d:%02d  ",tm->tm_year+1900,
				tm->tm_mon+1,tm->tm_mday,tm->tm_hour,tm->tm_min,tm->tm_sec);
				strcat(buffer,tmp);
				strcat(buffer,ent.nombre);
				strcat(buffer,"\n");
			}
		}
	}
	return nentradas;
}

int mi_chmod(const char *camino, unsigned char modo){
	unsigned int p_inodo_dir=0;
	unsigned int p_inodo= 0;
	unsigned int p_entrada = 0;

	int bEnt = buscar_entrada(camino,&p_inodo_dir,&p_inodo,&p_entrada,'0','0');

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
		case -4:
			printf("No existe");
			break;
		}
	}else{
		mi_chmod_f(p_inodo,modo);
	}
	return 0;
}

int mi_stat(const char *camino, struct STAT *p_stat){
	unsigned int p_inodo_dir=0;
	unsigned int p_inodo= 0;
	unsigned int p_entrada = 0;

	int bEnt = buscar_entrada(camino,&p_inodo_dir,&p_inodo,&p_entrada,'0','0');

	if(bEnt<0){
		switch(bEnt){
		case -1:
			printf("Tipo no adecuado \n");
			break;
		case -2:
			printf("No se puede leer el inodo \n");
			break;
		case -3:
			printf("El directorio donde apunta p_inodo_dir no tiene permisos de escritura \n");
			break;
		case -4:
			printf("No existe \n");
			break;
		}
		return -1;
	}else{
		mi_stat_f(p_inodo, p_stat);
	}
	return 0;
}

int mi_read(const char *camino, void *buf, unsigned int offset, unsigned int nbytes){
	unsigned int p_inodo_dir=0;
	unsigned int p_inodo= 0;
	unsigned int p_entrada = 0;

	int bEnt = buscar_entrada(camino,&p_inodo_dir,&p_inodo,&p_entrada,'0','0');
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
			case -4:
				printf("No existe");
				break;
			}
		}else{
			mi_read_f(p_inodo,buf,offset,nbytes);
	}
	return 0;
}

int mi_write(const char *camino, const void *buf, unsigned int offset, unsigned int nbytes){
	unsigned int p_inodo_dir=0;
	unsigned int p_inodo= 0;
	unsigned int p_entrada = 0;

	if(!sem){
		obtenerSem(&sem);
	}
	waitSem(sem);
	int bEnt = buscar_entrada(camino,&p_inodo_dir,&p_inodo,&p_entrada,'0','0');
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
			case -4:
				printf("No existe");
				break;
			}
			signalSem(sem);
			return -1;
		}else{
			mi_write_f(p_inodo,buf,offset,nbytes);
		}
	signalSem(sem);
	return 0;
}


