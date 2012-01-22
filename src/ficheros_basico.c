/*
 * ficheros_basico.h
 *
 *  Created on: 11/10/2011
 *      Author: manuel
 */

// fichero ficheros_basico.h

#include "ficheros_basico.h"

//Calcula el tamaño del mapa de bits
int tamMB (unsigned int nbloques){
	if (nbloques % (blocksize*8)==0){
		return nbloques/(blocksize*8);
	}else{
		return (nbloques/(blocksize*8))+1;
	}
}

//Calcula el tamaño del array de inodos
int tamAI (unsigned int ninodos){
	if (((ninodos*128)%blocksize)==0){
		return (ninodos*128)/blocksize;
	}else{
		return ((ninodos*128)/blocksize)+1;
	}
}

//Inicializa el superbloque
int initSB(unsigned int nbloques, unsigned int ninodos){
	struct superbloque sp;
	sp.posPrimerBloqueMB=posSB + 1; //Posición del primer bloque del mapa de bits
	sp.posUltimoBloqueMB = tamMB(nbloques); //Posición del último bloque del mapa de bits
	sp.posPrimerBloqueAI = sp.posUltimoBloqueMB+1; //Posición del primer bloque del array de inodos
	sp.posUltimoBloqueAI = tamAI(ninodos)+sp.posUltimoBloqueMB; //Posición del último bloque del array de inodos
	sp.posPrimerBloqueDatos = sp.posUltimoBloqueAI+1; //Posición del primer bloque de datos
	sp.posUltimoBloqueDatos = nbloques-1; //Posición del último bloque de datos
	sp.posInodoRaiz = 0; //Posición del inodo del directorio raíz
	sp.posPrimerInodoLibre = 0; //Posición del primer inodo libre
	sp.cantBloquesLibres = nbloques;  //Cantidad de bloques libres
	sp.cantInodosLibres = ninodos-1; //Cantidad de inodos libres
	sp.totBloques = nbloques; //Cantidad total de bloques
	sp.totInodos = ninodos; //Cantidad total de inodos

	if(bwrite(posSB,&sp)==-1){	//Escribe superbloque en el primer bloque
		return -1;
	}else{
		return 0;
	}

}

//Inicializa mapa de bits
int initMB(unsigned int nbloques){
	int i;
	unsigned char buf[blocksize];
	struct superbloque sb;
	int tMB = tamMB(nbloques);

	//Rellena a 0 el mapa de bits
	memset(buf,0,blocksize);
	for (i=1; i<=tMB; i++){
		if(bwrite(i,buf)==-1){
			return -1;
		}
	}
	bread(posSB,&sb);		//Lectura superbloque
	int numMB = sb.posUltimoBloqueMB-sb.posPrimerBloqueMB;
	for (i=sb.posPrimerBloqueMB; i<=sb.posUltimoBloqueMB ;i++){	//Marca bits del propio MB
		escribir_bit(i,1);
	}
	escribir_bit(posSB,1);
	sb.cantBloquesLibres-=numMB+1; //Aquí también resta el sb
	bwrite(posSB,&sb);	//Guarda el SB modificado
	return 0;
}

//Inicializa array de inodos
int initAI(unsigned int inodos){
	struct superbloque sb;
	int i,j,tamino, x;
	tamino = blocksize/T_INODO;
	struct inodo Ainodes[tamino];
	struct inodo Ainodes1[tamino];

	bread(posSB,&sb);
	x = 1;
	for (i=sb.posPrimerBloqueAI; i<=sb.posUltimoBloqueAI;i++){	//Llena el AI de inodos libres y los enlaza
		for (j=0;j<tamino;j++){
			Ainodes[j].punterosDirectos[0]=x;
			Ainodes[j].tipo='l';
			x++;
		}
		if (i==sb.posUltimoBloqueAI){
			Ainodes[7].punterosDirectos[0]=999999;	//Último inodo
		}
		if(bwrite(i,&Ainodes)==-1){	//Escribe array de inodos
			return -1;
		}
	}

	int numAI = sb.posUltimoBloqueAI-sb.posPrimerBloqueAI;
	for (i=sb.posPrimerBloqueAI; i<=sb.posUltimoBloqueAI ;i++){	//Marca AI en el MB
		escribir_bit(i,1);
	}
	sb.cantBloquesLibres-=numAI;
	bwrite(posSB,&sb);	//Guarda el SB modificado
	return 0;

}
//Marca bit en el MB
int escribir_bit(unsigned int nbloque, unsigned int bit){
	int mb,pos_byte,pos_bit,posMB;
	unsigned char mapa_bits[blocksize];

	struct superbloque sb;
	bread(posSB,&sb);		//Lectura superbloque

	mb = sb.posPrimerBloqueMB;
	pos_byte= nbloque / 8;
	pos_bit= nbloque % 8;
	posMB = (pos_byte/blocksize)+mb;

	bread(posMB,&mapa_bits);

	pos_byte = pos_byte%blocksize;

	unsigned char mascara = 128; // 10000000
	mascara >>= pos_bit; // desplazamiento de bits a la derecha
	if (bit==1){
		mapa_bits[pos_byte] |= mascara; // operador OR para bits
	}else if(bit==0){
		mapa_bits[pos_byte] &= ~mascara; // operadores AND y NOT para bits
	}else{
		return -1;
	}
	bwrite(posMB,&mapa_bits);	//Guarda el SB modificado
	return 0;
}
//Lee bit en el MB
unsigned char leer_bit(unsigned int nbloque){
	int mb,pos_byte,pos_bit,posMB;
	unsigned char mapa_bits[blocksize];

	struct superbloque sb;
	bread(posSB,&sb);		//Lectura superbloque

	mb = sb.posPrimerBloqueMB;
	pos_byte= nbloque / 8;
	pos_bit= nbloque % 8;

	posMB = (pos_byte/blocksize)+mb;
	bread(posMB,&mapa_bits);

	pos_byte = pos_byte%blocksize;

	unsigned char resultado = 128; // 10000000
	resultado >>= pos_bit; // desplazamiento de bits a la derecha
	resultado &= mapa_bits[pos_byte]; // operador AND para bits
	resultado >>= (7-pos_bit); // desplazamiento de bits a la derecha
	return resultado;
}
//Reserva un bloque para un inodo
int reservar_bloque(){
	struct superbloque sb;
	int n,pos_byte,pos_bit,i,r;
	unsigned char byte;
	unsigned char mapa_bits[blocksize];
	unsigned char bufferAux[blocksize];

	bread(posSB,&sb);		//Lectura superbloque

	if(sb.cantBloquesLibres>0){	//Busca primer bloque libre
		bread(sb.posPrimerBloqueMB,mapa_bits);
		n = sb.posPrimerBloqueMB;
		memset (bufferAux, 255, blocksize);
		do{
			if(memcmp(mapa_bits, bufferAux, blocksize)<0){
				break;
			}else{
				n++;
				bread(n,mapa_bits);
			}
		}while(n<=sb.posUltimoBloqueMB);

		pos_byte= 0;
		pos_bit= 0;

		unsigned char mascara = 128; // 10000000

		for(i=0;i<=1024;i++){
			byte = mapa_bits[i];
			if (byte < 255) { // hay bits a 0 en el byte
				while (byte & mascara) { // operador AND para bits
					byte <<= 1; // desplazamiento de bits a la izquierda
					pos_bit++;
				}
				break;
			}
		}
		r = ((n-sb.posPrimerBloqueMB)*blocksize+i)*8+pos_bit;
		escribir_bit(r,1);
		sb.cantBloquesLibres-=1;
		bwrite(posSB,&sb);	//Guarda el SB modificado
		return r;
	}else{
		printf("No quedan bloques libres");
		return -1;
	}
}

int liberar_bloque(unsigned int nbloque){
	escribir_bit(nbloque,0);	//Marca como vacío en el SB
	struct superbloque sb;
	bread(posSB,&sb);		//Lectura superbloque
	sb.cantBloquesLibres+=1;	//Aumenta los bloques libres
	bwrite(posSB,&sb);	//Escribe SB modificado
	return nbloque;
}

int escribir_inodo(struct inodo inodo, unsigned int ninodo){
	struct superbloque sb;
	int ino,tamino;
	bread(posSB,&sb);		//Lectura superbloque
	ino = sb.posPrimerBloqueAI;
	tamino = blocksize/T_INODO;
	ino = ninodo/tamino+ino;
	struct inodo Ainodes[tamino];
	bread(ino,&Ainodes);	//Lectura de array de inodos
	int conta = ninodo % tamino;
	Ainodes[conta]=inodo;
	bwrite(ino,&Ainodes);	//Escritura de inodo
	return 0;
}

struct inodo leer_inodo(unsigned int ninodo){
	struct superbloque sb;
	int ino,tamino;
	struct inodo inodo;
	bread(posSB,&sb);		//Lectura superbloque
	ino = sb.posPrimerBloqueAI;
	tamino = blocksize/T_INODO;
	int conta = ninodo%tamino;
	ino = ninodo/tamino+ino;
	struct inodo Ainodes[tamino];
	bread(ino,&Ainodes);	//Lectura de array de inodos
	inodo=Ainodes[conta];
	return inodo;
}

int reservar_inodo(unsigned char tipo, unsigned char permisos){
	struct superbloque sb;
	int illiure,i,tamino,Pino,nbloque;
	bread(posSB,&sb);		//Lectura superbloque
	illiure = sb.posPrimerInodoLibre; //Debe pasar a bloque
	Pino = sb.posPrimerBloqueAI;
	struct inodo in;
	tamino = blocksize/T_INODO;
	nbloque = illiure/tamino+Pino;	//Cálculo del bloque

	in.tipo=tipo;
	in.permisos=permisos;
	in.nlinks=1;
	in.tamEnBytesLog=0;
	in.atime=time(NULL);
	in.ctime=time(NULL);
	in.mtime=time(NULL);
	in.numBloquesOcupados=0;
	for(i=0;i<=11;i++){		//Llena inodo y inicializa los punteros a 0
		in.punterosDirectos[i]=0;
	}
	for(i=0;i<=2;i++){
		in.punterosIndirectos[i]=0;
	}

	struct inodo Ainodes[tamino];
	bread(nbloque,&Ainodes);	//Lectura de array de inodos
	int conta = illiure%tamino;
	sb.posPrimerInodoLibre=Ainodes[conta].punterosDirectos[0];	//Enlaza al nuevo inodo libre
	sb.cantInodosLibres--;	//Un inodo libre menos

	Ainodes[conta]=in;
	escribir_inodo(in,illiure);	//Escribe el nuevo inodo
	bwrite(posSB,&sb);	//Actualiza el SB
	return illiure;
}

int traducir_bloque_inodo(unsigned int ninodo, unsigned int blogico, unsigned int *bfisico, char reservar){
	int pdir,npun,npun2,npun3,i,punt0,punt1,punt2;
	struct inodo in;

	pdir = 12;
	npun = 256;
	unsigned int bufferIndirectos0[npun];
	unsigned int bufferIndirectos1[npun];
	unsigned int bufferIndirectos2[npun];
	npun2 = npun*npun;
	npun3 = npun*npun2;

	in = leer_inodo(ninodo);

	if (blogico < pdir){ //Punteros directos
		switch (reservar){
		case '0':	//Lectura
			if (in.punterosDirectos[blogico]==0){	//No existe
				return -1;
			}else{
				*bfisico = in.punterosDirectos[blogico];	//Devuelve el bloque fisico
				return 0;
			}
			break;
		case '1':	//Escritura
			if (in.punterosDirectos[blogico]==0){	//Si no existe reserva uno nuevo
				in.punterosDirectos[blogico] = reservar_bloque();
				in.numBloquesOcupados++;
				*bfisico=in.punterosDirectos[blogico];
				escribir_inodo(in,ninodo);
			}else{
				*bfisico = in.punterosDirectos[blogico];	//Devuelve el bloque físico
			}
			break;
		default:	//Número entrado por error
			return -2;
			break;
		}
	}else if(blogico<(pdir+npun)){ //Punteros indirectos 0
		switch(reservar){
		case '0':	//Lectura
			if (in.punterosIndirectos[0] == 0){	//No existe
				return -1;
			}else{
				if (bread(in.punterosIndirectos[0],bufferIndirectos0)==-1){	//No existe array de punteros
					return -1;
				}else{
					*bfisico = bufferIndirectos0[blogico-pdir];	//Devuelve el bloque físico
					return 0;
				}
			}
			break;
		case '1':	//Escritura
			if (in.punterosIndirectos[0]==0){	//Si no existe crea array de punteros0 y bloque final
				for (i=0; i<npun;i++){
					bufferIndirectos0[i]=0;
				}
				bufferIndirectos0[blogico-pdir] = reservar_bloque();
				in.numBloquesOcupados++;
				in.punterosIndirectos[0]= reservar_bloque();
				bwrite(in.punterosIndirectos[0],bufferIndirectos0);
				*bfisico = bufferIndirectos0[blogico-pdir];	//Devuelve el bloque físico
			}else{
				bread(in.punterosIndirectos[0],bufferIndirectos0);	//Si no existe reserva bloque
				if(bufferIndirectos0[blogico-pdir]==0){
					bufferIndirectos0[blogico-pdir] = reservar_bloque();
					in.numBloquesOcupados++;
					bwrite(in.punterosIndirectos[0],bufferIndirectos0);
					*bfisico = bufferIndirectos0[blogico-pdir];
				}else{
					*bfisico = bufferIndirectos0[blogico-pdir];	//Devuelve el bloque físico
				}
			}
			escribir_inodo(in,ninodo);	//Escribe el inodo modificado
			break;
		default:
			return -2;
			break;
		}
	}else if (blogico < pdir+npun+npun2){ //Punteros indirectos 1
		punt0 = (blogico -(pdir+npun)) %npun;
		punt1 = (blogico -(pdir+npun)) /npun;
		switch(reservar){
		case '0':	//Lectura
			if(in.punterosIndirectos[1] == 0){	//No existe array de punteros1
				return -1;
			} else {
				bread(in.punterosIndirectos[1],bufferIndirectos1);
				if (bufferIndirectos1[punt1]==0){	//No existe array de punteros0
					return -4;
				}else{
					bread(in.punterosIndirectos[0],bufferIndirectos0);
					if (bufferIndirectos0[punt0]==0){	//No existe bloque de datos
						return -1;
					}else{
						*bfisico = bufferIndirectos0[punt0];	//Devuelve bloque físico
					}
				}
			}
			break;
		case '1':	//Escritura
			if (in.punterosIndirectos[1]==0){	////Si no existe crea array de punteros0,1 y bloque final
				in.punterosIndirectos[1] = reservar_bloque();
				for (i=0; i<npun;i++){
					bufferIndirectos1[i]=0;
				}
				bufferIndirectos1 [punt1]= reservar_bloque();
				for (i=0; i<npun;i++){
					bufferIndirectos0[i]=0;
				}
				bufferIndirectos0[punt0] = reservar_bloque();
				in.numBloquesOcupados++;
				bwrite(in.punterosIndirectos[1], bufferIndirectos1);
				bwrite(bufferIndirectos1[punt1], bufferIndirectos0);
				*bfisico = bufferIndirectos0[punt0];	//Devuelve bloque físico
			}else{
				bread(in.punterosIndirectos[1],bufferIndirectos1);
				if(bufferIndirectos1[punt1]==0){	//Si no existe crea array de punteros0 y bloque final
					bufferIndirectos1[punt1]=reservar_bloque();
					for (i=0; i<npun;i++){
						bufferIndirectos0[i]=0;
					}
					bufferIndirectos0[punt0]=reservar_bloque();
					in.numBloquesOcupados++;
					bwrite(in.punterosIndirectos[1], bufferIndirectos1);
					bwrite(bufferIndirectos1[punt1], bufferIndirectos0);
					*bfisico = bufferIndirectos0[punt0];
				}else{
					bread(bufferIndirectos1[punt1],bufferIndirectos0);
						if (bufferIndirectos0[punt0]==0){	//Si no existe reserva bloque
							bufferIndirectos0[punt0] = reservar_bloque();
							in.numBloquesOcupados++;
							bwrite(bufferIndirectos1[punt1], bufferIndirectos0);
							*bfisico = bufferIndirectos0[punt0];
						}else{
							*bfisico = bufferIndirectos0[punt0];	//Devuelve bloque físico
						}
				}

			}
			escribir_inodo(in,ninodo);	//Escribe bloque modificado
			break;
		}
	}else if(blogico < pdir+npun+npun2+npun3){ //Punteros indirectos 2
		punt2 = (blogico - (pdir+npun+npun2))/npun2;
		punt1 = ((blogico -(pdir+npun+npun2)) %npun2)/npun;
		punt0 = ((blogico -(pdir+npun+npun2))% npun2)%npun;
		switch(reservar){
		case '0':	//Lectura
			if(in.punterosIndirectos[2] == 0){	//No existe array de punteros2
				return -1;
			}else{
				bread(in.punterosIndirectos[2],bufferIndirectos2);	//No existe array de punteros1
				if (bufferIndirectos2[punt2]==0){
					return -1;
				}else{
					bread(bufferIndirectos2[punt2],bufferIndirectos1);	//No existe array de punteros0
					if (bufferIndirectos1[punt1]==0){
						return -1;
					}else{
						bread(bufferIndirectos1[punt1],bufferIndirectos0);	//No existe bloque de datos
						if (bufferIndirectos0[punt0]==0){
							return -1;
						}else{
							*bfisico = bufferIndirectos0[punt0];	//Devuelve bloque de datos
						}
					}
				}
			}
		break;
		case '1':	//Escritura
			if(in.punterosIndirectos[2] == 0){	//Si no existe crea array de punteros2,1,0 y bloque de datos
				in.punterosIndirectos[2] = reservar_bloque();
				for (i=0; i<npun;i++){
					bufferIndirectos2[i]=0;
				}
				bufferIndirectos2[punt2] = reservar_bloque();
				for (i=0; i<npun;i++){
					bufferIndirectos1[i]=0;
				}
				bufferIndirectos1[punt1] = reservar_bloque();
				for (i=0; i<npun;i++){
					bufferIndirectos0[i]=0;
				}
				bufferIndirectos0[punt0] = reservar_bloque();
				in.numBloquesOcupados++;
				bwrite(in.punterosIndirectos[2], bufferIndirectos2);
				bwrite(bufferIndirectos2[punt2], bufferIndirectos1);
				bwrite(bufferIndirectos1[punt1], bufferIndirectos0);
				*bfisico = bufferIndirectos0[punt0];	//Devuelve bloque de datos
			}else{
				bread(in.punterosIndirectos[2],bufferIndirectos2); //Si no existe crea array de punteros2,0 y bloque de datos
				if(bufferIndirectos2[punt2]==0){
					bufferIndirectos2[punt2] = reservar_bloque();
					for (i=0; i<npun;i++){
						bufferIndirectos1[i]=0;
					}
					bufferIndirectos1[punt1] = reservar_bloque();
					for (i=0; i<npun;i++){
						bufferIndirectos0[i]=0;
					}
					bufferIndirectos0[punt0] = reservar_bloque();
					in.numBloquesOcupados++;
					bwrite(in.punterosIndirectos[2], bufferIndirectos2);
					bwrite(bufferIndirectos2[punt2], bufferIndirectos1);
					bwrite(bufferIndirectos1[punt1], bufferIndirectos0);
					*bfisico = bufferIndirectos0[punt0];	//Devuelve bloque de datos
				}else{
					bread(bufferIndirectos2[punt2],bufferIndirectos1);
					if (bufferIndirectos1[punt1]== 0){	//Si no existe crea array de punteros0 y bloque de datos
						bufferIndirectos1[punt1] = reservar_bloque();
						for (i=0; i<npun;i++){
							bufferIndirectos0[i]=0;
						}
						bufferIndirectos0[punt0] = reservar_bloque();
						in.numBloquesOcupados++;
						bwrite(bufferIndirectos2[punt2], bufferIndirectos1);
						bwrite(bufferIndirectos1[punt1], bufferIndirectos0);
						*bfisico = bufferIndirectos0[punt0];	//Devuelve el bloque de datos
					}else{
						bread(bufferIndirectos1[punt1],bufferIndirectos0);
						if (bufferIndirectos0[punt0] == 0){	//Si no existe crea el bloque de datos
							bufferIndirectos0[punt0] = reservar_bloque();
							in.numBloquesOcupados++;
							bwrite(bufferIndirectos1[punt1], bufferIndirectos0);
							*bfisico = bufferIndirectos0[punt0];
						}else{
							*bfisico = bufferIndirectos0[punt0];	//Devuelve bloque de datos
						}
					}
				}
			}
			escribir_inodo(in,ninodo);
			break;
		}
	}
	return 0;
}

int liberar_inodo(unsigned int ninodo){
	struct superbloque sb;
	struct inodo in;

	bread(posSB,&sb);	//Lee el SB
	in = leer_inodo(ninodo);
	liberar_bloques_inodo(ninodo,0);
	in.tipo='l';
	in.punterosDirectos[0]=sb.posPrimerInodoLibre;	//Enlaza el primer inodo libre
	sb.posPrimerInodoLibre=ninodo;	//Pone el inodo liberado como el primero libre
	sb.cantInodosLibres++;
	escribir_inodo(in,ninodo);
	bwrite(posSB,&sb);	//Escribe SB modificado
	return ninodo;
}

int liberar_bloques_inodo(unsigned int ninodo, unsigned int blogico){
	struct inodo in;

	int i,blog,punt0,punt1,punt2;
	int num_pDirectos = 12;
	int num_punteros = 256;
	int num_punteros2 = num_punteros * num_punteros;
	int num_punteros3 = num_punteros2 * num_punteros;
	int bufferIndirectos[num_punteros];
	int bufferIndirectos0[num_punteros];
	int bufferIndirectos1[num_punteros];
	int bufferIndirectos2[num_punteros];

	in = leer_inodo(ninodo);
	int ultimoBloque = in.tamEnBytesLog/blocksize;

	for (i=0; i<num_punteros;i++){	//Inicializa punteros
		bufferIndirectos[i]=0;
	}

	for (blog=blogico;blog <= ultimoBloque; blog++){	//Recorre y libera los bloques
		if (blog< num_pDirectos){	//Si está en los bloques directos
			if (in.punterosDirectos[blog] > 0){	//Si existe libera el bloque
				liberar_bloque(blog);
				in.punterosDirectos[blog] = 0;
				in.numBloquesOcupados--;
			}
		}
		else if(blog < num_pDirectos + num_punteros){ //Si está en los punteroindirectos0
			if (in.punterosIndirectos[0]==0){	//No existe indirectos0
				return -1;
			}else{
				bread(in.punterosIndirectos[0],bufferIndirectos0);
				if (bufferIndirectos0[blog-num_pDirectos]==0){	//No existe el bloque
					return -1;
				}else{	//Libera el bloque de datos y actualiza indirectos0
					liberar_bloque(bufferIndirectos0[blog-num_pDirectos]);
					bufferIndirectos0[blog-num_pDirectos]=0;
					in.numBloquesOcupados--;
					if (memcmp(bufferIndirectos0, bufferIndirectos, num_punteros)==0){	//Si indirectos0 está vacío lo libera
						liberar_bloque(in.punterosIndirectos[0]);
						in.punterosIndirectos[0]=0;
					}else{
						bwrite(in.punterosIndirectos[0],bufferIndirectos0);	//Guarda indirectos0 modificado
					}
				}
			}
		}
		else if(blog<num_pDirectos+num_punteros+num_punteros2){	//Si está en indirectos1
			punt0 = (blog - (num_pDirectos + num_punteros)) % num_punteros;
			punt1 = (blog - (num_pDirectos + num_punteros)) / num_punteros;
			if (in.punterosIndirectos[1]==0){	//No existe indirectos1
				return -1;
			}else{
				bread(in.punterosIndirectos[1],bufferIndirectos1);
				if (bufferIndirectos1[punt1]==0){	//No existe array de indirectos0
					return -1;
				}else{
					bread(bufferIndirectos1[punt1],bufferIndirectos0);
					if (bufferIndirectos0[punt0]==0){ //No existe bloque
						return -1;
					}else{	//Libera el bloque de datos y actualiza indirectos1 y 0
						liberar_bloque(bufferIndirectos0[punt0]);	//Libera bloque de datos
						bufferIndirectos0[punt0]=0;
						in.numBloquesOcupados--;
						if (memcmp(bufferIndirectos0, bufferIndirectos, num_punteros)==0){	//Si vacío libera indirectos0
							liberar_bloque(bufferIndirectos1[punt1]);
							bufferIndirectos1[punt1]=0;
						}else{	//Escribe indirectos0 modificado
							bwrite(bufferIndirectos1[punt1],bufferIndirectos0);
						}
						if (memcmp(bufferIndirectos1, bufferIndirectos, num_punteros)==0){ //Si vacío libera indirectos1
							liberar_bloque(in.punterosIndirectos[1]);
							in.punterosIndirectos[1]=0;
						}else{	//Escribe indirectos1 modificado
							bwrite(in.punterosIndirectos[1],bufferIndirectos1);
						}
					}
				}
			}
		}
		else if(blog < num_pDirectos + num_punteros + num_punteros2 + num_punteros3){	//Si está en indirectos2
			punt0 = ((blog - (num_pDirectos + num_punteros + num_punteros2)) % num_punteros2) % num_punteros;
			punt1 = ((blog - (num_pDirectos + num_punteros + num_punteros2)) % num_punteros2) / num_punteros;
			punt2 = (blog - (num_pDirectos + num_punteros + num_punteros2)) / num_punteros2;
			if (in.punterosIndirectos[2]==0){ //Si no existe indirectos2
				return -1;
			}else{
				bread(in.punterosIndirectos[2],bufferIndirectos2);
				if (bufferIndirectos2[punt2]==0){	//Si no existe indirectos1
					return -1;
				}else{
					bread(bufferIndirectos2[punt2],bufferIndirectos1);
					if (bufferIndirectos1[punt1]==0){	//Si no existe indirectos0
						return -1;
					}else{
						bread(bufferIndirectos1[punt1],bufferIndirectos0);
						if (bufferIndirectos0[punt0]==0){	//Si no existe el bloque
							return -1;
						}else{
							liberar_bloque(bufferIndirectos0[punt0]);	//Libera el bloque
							bufferIndirectos0[punt0]=0;
							in.numBloquesOcupados--;
							if (memcmp(bufferIndirectos0, bufferIndirectos, num_punteros)==0){	//Si vacío libera indirectos0
								liberar_bloque(bufferIndirectos1[punt1]);
								bufferIndirectos1[punt1]=0;
							}else{	//Escribe indirectos0 modificado
								bwrite(bufferIndirectos1[punt1],bufferIndirectos0);
							}
							if (memcmp(bufferIndirectos1, bufferIndirectos, num_punteros)==0){	//Si vacío libera indirectos1
								liberar_bloque(bufferIndirectos2[punt2]);
								bufferIndirectos2[punt2]=0;
							}else{	//Escribe indirectos1 modificado
								bwrite(bufferIndirectos2[punt2],bufferIndirectos1);
							}
							if (memcmp(bufferIndirectos2, bufferIndirectos, num_punteros)==0){	//Si vacío libera indirectos2
								liberar_bloque(in.punterosIndirectos[2]);
								in.punterosIndirectos[2]=0;
							}else{	//Escribe indirectos2 modificado
								bwrite(in.punterosIndirectos[2],bufferIndirectos2);
							}
						}
					}
				}
			}
		}
	}
	escribir_inodo(in,ninodo);	//Escribe inodo modificado
	return 0;
}





