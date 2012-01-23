/*
 * directorios.c
 *
 *  Created on: 22/11/2011
 *      Author: manuel
 */
#include "directorios.h"
int sem = 0;

//Separa un camino en inicial y final
int extraer_camino(const char *camino, char *inicial, char *final){
	int tam,n,r;
	if (camino[0]!='/'){	//Si no es empieza por / no es correcto
		return -1;
	}
	n=1;
	while ((camino[n]!='/') && camino[n]!='\0'){	//Copia a inicial
		inicial[n-1] = camino[n];
		n++;
	}
	tam = strlen(camino);
	if (tam == n){	//Si hemos llegado al final es un fichero
		final='\0';
		return 0; //fichero
	}
	r=0;
	while (n<tam){	//Copia a final el resto de camino
		final[r] = camino[n];
		n++;
		r++;
	}
	return 1; //directorio
}

//Busca recursivamente una entrada
int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir, unsigned int *p_inodo, unsigned int *p_entrada, char reservar, unsigned char modo){
	char inicial[60],final[60];
	struct inodo in;
	struct entrada ent;
	int numentradas,nentrada,tipo;
	if (strcmp(camino_parcial,"/")==0){	//Raiz
		*p_inodo=0;
		*p_entrada=0;
		return 0;
	}

	memset(inicial,'\0',60);
	memset(final,'\0',60);

	tipo = extraer_camino(camino_parcial,inicial,final);	//Separa inicial y final
	if (tipo==-1){
		return -1;
	}
	in = leer_inodo(*p_inodo_dir);	//Lee el directorio que lo contiene para leer su entrada
	memset(ent.nombre,'\0',60);
	numentradas = in.tamEnBytesLog/sizeof(struct entrada);
	nentrada = 0;
	if(numentradas>0){
		if(mi_read_f(*p_inodo_dir,&ent,0,sizeof(struct entrada)) == -1){	//Lee la primera entrada
			return -2; //No permisos de lectura
		}
		while ((nentrada<numentradas) && (strcmp(inicial,ent.nombre))){	//Recorre las entradas hasta que acabe o encuentre el nombre
			nentrada++;
			if(mi_read_f(*p_inodo_dir,&ent,nentrada*sizeof(struct entrada),sizeof(struct entrada))==-1){
				return -2;
			}
		}
	}
	if(nentrada==numentradas){
		switch (reservar){
		case '0':	//Consulta
			return -4; //Modo consulta, no existeix
			break;
		case '1':	//Escritura
			strcpy(ent.nombre,inicial);	//Copia el nombre en la entrada y reserva un inodo
			if(tipo==1){	//Directorio
				if(!strcmp(final,"/")){
					ent.inodo = reservar_inodo('d',modo);
				}else{
					ent.inodo = reservar_inodo('d',7);
				}
			}else{	//Fichero
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
	if(strcmp(final,"/") == 0 || strcmp(final,"") == 0){	//Si ya es la final devuelve
		*p_inodo = ent.inodo;
		*p_entrada = nentrada;
		if((nentrada<numentradas) && (reservar=='1')){
			return -5; //Modo escritura y ya existe
		}
		return 0;
	}else{	//Si no es final extrae de nuevo el camino
		*p_inodo_dir = ent.inodo;
		buscar_entrada(final,p_inodo_dir,p_inodo,p_entrada,reservar,modo);
	}
	return 0;
}

//Crea un fichero/directorio
int mi_creat(const char *camino, unsigned char modo){
	unsigned int p_inodo_dir=0;
	unsigned int p_inodo= 0;
	unsigned int p_entrada = 0;
	int bEnt;
	if(!sem){	//Obtiene semáforo
		obtenerSem(&sem);
	}
	waitSem(sem);
	bEnt= buscar_entrada(camino, &p_inodo_dir, &p_inodo,&p_entrada,'1',modo); //Escribe
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
	return 0;
}

//Enlaza una entrada con otra
int mi_link(const char *camino1, const char *camino2){
	unsigned int p_inodo_dir = 0;
	unsigned int p_inodo = 0;
	unsigned int p_entrada = 0;
	struct entrada entrada;
	struct inodo inodo;

	if(!sem){	//Obtener semáforo
		obtenerSem(&sem);
	}
	waitSem(sem);
	int rest = buscar_entrada(camino1,&p_inodo_dir,&p_inodo,&p_entrada,'0','0'); //Obtiene entrada origen
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
	int bEnt = buscar_entrada(camino2,&p_inodo_dir,&p_inodo,&p_entrada,'1','0');	//Crea entrada destino

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
		if(mi_read_f(p_inodo_dir,&entrada,p_entrada*sizeof(struct entrada),sizeof(struct entrada))==-1){	//Si no se puede leer
			signalSem(sem);
			return -2;
		}	//Libera inodo comodín y enlaza al otro
		liberar_inodo(entrada.inodo);
		entrada.inodo = ninodo;
		if(mi_write_f(p_inodo_dir,&entrada,p_entrada*sizeof(struct entrada),sizeof(struct entrada))==-1){	//Escribe entrada
			signalSem(sem);
			return -3; //El directorio donde apunta p_inodo_dir no tiene permisos de escritura
		}else{
			inodo = leer_inodo(ninodo);
			inodo.nlinks++;
			inodo.ctime = time(NULL);
			escribir_inodo(inodo,ninodo);	//Escribe inodo modificado
		}
	}
	signalSem(sem);
	return 0;
}

//Desenlaza una entrada
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
		if((inodoF.tipo=='d') && (inodoF.tamEnBytesLog>=64) && (inodoF.nlinks==1)){
			printf("El directorio no está vacío \n");
			return -5;
		}
		if (nentradas-1==p_entrada){	//Si es la última entrada la borra
			mi_truncar_f(p_inodo_dir,inodo.tamEnBytesLog-sizeof(struct entrada));
		}else{	//Mueve la última entrada a su posición y borra la última
			mi_read_f(p_inodo_dir, &buf_com, (nentradas-1)*sizeof(struct entrada), sizeof(struct entrada));
			mi_write_f(p_inodo_dir, &buf_com, p_entrada*sizeof(struct entrada), sizeof(struct entrada));
			mi_truncar_f(p_inodo_dir,inodo.tamEnBytesLog-sizeof(struct entrada));
		}
		inodoF.nlinks--;
		if (inodoF.nlinks==0){	//Si no tiene más links borra el inodo
			liberar_inodo(buf_original.inodo);
		}else{	//Actualiza inodo
			inodoF.ctime=time(NULL);
			escribir_inodo(inodoF,buf_original.inodo);
		}
	}
	signalSem(sem);
	return 0;
}

//Lista un directorio
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
		if((inodo.tipo=='d') && (inodo.permisos & 4)){	//Si es un directorio y tiene permisos de lectura
			nentradas = inodo.tamEnBytesLog/sizeof(struct entrada);
			for (i=0; i<nentradas;i++){	//Itera todos los registros y los junta en un string
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
				tm = localtime(&inodo.mtime);
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

//Cambia permisos de un fichero/directorio
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
		mi_chmod_f(p_inodo,modo);	//Cambia permisos
	}
	return 0;
}

//Extrae datos de una entrada
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

//Lee el contenido de un fichero/directorio
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

//Escribe en un fichero/directorio
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


