/*
 * simulacion.c
 *
 *  Created on: 16/01/2012
 *      Author: manuel
 */


#include <sys/wait.h>
#include <sys/resource.h>
#include "directorios.h"

struct Registro {
	int hora;
	int npid;
	int nesc;
	int nreg;
};

#define PROC 14
#define ESCR 50
int nproces = 0;

void enterrador(int i){
	while(wait3(NULL, WNOHANG, NULL) > 0){
		nproces++;
	}
	printf("%d procesos terminados\n",nproces);
}

int proceso(int n, char *direc){
	int numpid,i,aleat;
	char direc1[60];
	char fichero[60];
	struct Registro reg;

	numpid = getpid();
	sprintf(direc1,"%sproceso_%d/",direc,numpid);
	if(mi_creat(direc1,7)<0){
		printf("Error al crear directorio \n");
	}else{
		printf("Directorio creado correctamente \n");
		sprintf(fichero,"%sprueba.dat",direc1);
		if(mi_creat(fichero,7)<0){
			printf("Error al crear fichero \n");
		}else{
			printf("Fichero creado correctamente \n");
			for(i=0;i<ESCR;i++){
				aleat=(random()%50)*sizeof(reg);
				reg.npid=numpid;
				reg.nesc=n+1;
				reg.nreg=aleat;
				reg.hora=time(NULL);
				if(mi_write(fichero, &reg, aleat, sizeof(reg))<0){
					printf("Fallo en la escritura \n");
					return -1;
				}else{
					printf("Registro %d escrito en posición %d del fichero %d \n",n+1,aleat,numpid);
				}
				usleep(50000);
			}
		}

	}

	return 0;
}

void verificar(char *direc){
	int numpid,n,i,numentradas,numreg,cont;
	struct Registro PriEsc,UltEsc,MenPos,MayPos,Reg;
	struct STAT p_stat,p_statreg;
	struct entrada ent;
	char fichero[120];
	char npid[10];

	PriEsc.hora=UltEsc.hora=0;
	MenPos.nreg=MayPos.nreg=0;

	memset(fichero,'\0',120);

	printf("\n\n");
	if(mi_stat(direc, &p_stat)<0){
		printf("Error al obtener los datos del directorio %s \n",direc);
	}else{
		numentradas = p_stat.tamEnBytesLog/sizeof(struct entrada);
		for(n=0;n<numentradas;n++){
			mi_read(direc,&ent,n*sizeof(struct entrada),sizeof(struct entrada));
			sprintf(fichero,"%s%s/prueba.dat",direc,ent.nombre);
			int tam=strlen(ent.nombre);

			for(n=8;n<tam;n++){
				npid[n-8]=ent.nombre[n];
			}
			numpid=atoi(npid);

			if(mi_stat(direc, &p_statreg)<0){
				printf("Error al obtener datos del fichero");
			}else{
				cont=0;
				numreg=p_statreg.tamEnBytesLog/sizeof(struct Registro);
				for(i=0;i<numreg;i++){
					mi_read(fichero,&Reg,i*sizeof(struct Registro),sizeof(struct Registro));
					if(Reg.npid==numpid){
						cont++;
						if(cont==1){
							PriEsc=Reg;
							UltEsc=Reg;
							MenPos=Reg;
							MayPos=Reg;
						}else{
							if(Reg.hora<PriEsc.hora){
								PriEsc=Reg;
							}
							if(Reg.hora>UltEsc.hora){
								UltEsc=Reg;
							}
							if(Reg.nreg<MenPos.nreg){
								MenPos=Reg;
							}
							if(Reg.nreg>MayPos.nreg){
								MayPos=Reg;
							}
						}
					}
					printf("Fichero con pid:%d y %d escrituras \n",numpid,cont);
					printf("Primera escritura. Hora:%d Nescritura:%d Nposicion:%d \n",PriEsc.hora,PriEsc.nesc,PriEsc.nreg);
					printf("Última escritura. Hora:%d Nescritura:%d Nposicion:%d \n",UltEsc.hora,UltEsc.nesc,UltEsc.nreg);
					printf("Menor posición. Nposicion:%d Nescritura:%d Hora:%d \n",MenPos.nreg,MenPos.nesc,MenPos.hora);
					printf("Mayor posición. Nposicion:%d Nescritura:%d Hora:%d \n\n",MayPos.nreg,MayPos.nesc,MayPos.hora);
				}
			}

		}
	}


}



int main(int argc, char **argv){

	char direc[60];
	int n;
	time_t hora;
	struct tm *tm;

	//Comprueba el número de argumentos
	if(argc!=2){
		printf("Número de argumentos incorrecto \n");
		return -1;
	}
	//Monta el Sistema de ficheros
	bmount(argv[1]);

	//Crea el directorio con la fecha y hora
	hora = time(NULL);
	tm = localtime(&hora);
	memset(direc,'\0',60);
	sprintf(direc,"/simul_%d%d%d%d%d%d/",tm->tm_year+1900,tm->tm_mon+1,tm->tm_mday,tm->tm_hour, tm->tm_min, tm->tm_sec);
	mi_creat(direc, 7);

	//Prepara enterrador e inicializa nproces
	signal(SIGCHLD, enterrador);
	nproces = 0;

	//Lanza los procesos con espera de 0,2 segs
	for(n=0;n<PROC;n++){
		if(fork() == 0){
			proceso(n,direc);
			exit(0);
		}
		usleep(200000);
	}

	//Para hasta que no acaben todos los procesos
	while(nproces<PROC){
		pause();
	}
	verificar(direc);

	//Desmonta el sistema de ficheros
	bumount();

	return 0;
}
