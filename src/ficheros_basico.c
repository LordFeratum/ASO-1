/*
 * ficheros_basico.h
 *
 *  Created on: 11/10/2011
 *      Author: manuel
 */

// fichero ficheros_basico.h

#include "ficheros_basico.h"

int tamMB (unsigned int nbloques){
	if (nbloques % (blocksize*8)==0){
		return nbloques/(blocksize*8);
	}else{
		return (nbloques/(blocksize*8))+1;
	}
}


int tamAI (unsigned int ninodos){
	if (((ninodos*128)%blocksize)==0){
		return (ninodos*128)/blocksize;
	}else{
		return ((ninodos*128)/blocksize)+1;
	}
}


int initSB(unsigned int nbloques, unsigned int ninodos){
	struct superbloque sp;
	sp.posPrimerBloqueMB=posSB + 1; //Posición del primer bloque del mapa de bits
	sp.posUltimoBloqueMB = tamMB(nbloques); //Posición del último bloque del mapa de bits
	sp.posPrimerBloqueAI = sp.posUltimoBloqueMB+1; //Posición del primer bloque del array de inodos
	sp.posUltimoBloqueAI = tamAI(ninodos)+sp.posUltimoBloqueMB; //Posición del último bloque del array de inodos
	sp.posPrimerBloqueDatos = sp.posUltimoBloqueAI+1; //Posición del primer bloque de datos
	sp.posUltimoBloqueDatos = nbloques-1; //Posición del último bloque de datos
	sp.posInodoRaiz = 0; //Posición del inodo del directorio raíz
	sp.posPrimerInodoLibre = 1; //Posición del primer inodo libre
	sp.cantBloquesLibres = nbloques;  //Cantidad de bloques libres
	sp.cantInodosLibres = ninodos-1; //Cantidad de inodos libres
	sp.totBloques = nbloques; //Cantidad total de bloques
	sp.totInodos = ninodos; //Cantidad total de inodos

	if(bwrite(posSB,&sp)==-1){
		return -1;
	}else{
		return 0;
	}

}

int initMB(unsigned int nbloques){
	int i;
	unsigned char buf[blocksize];
	struct superbloque sb;
	int tMB = tamMB(nbloques);



	memset(buf,0,blocksize);
	for (i=1; i<=tMB; i++){
		if(bwrite(i,buf)==-1){
			return -1;
		}
	}
	bread(posSB,&sb);		//Lectura superbloque
	int numMB = sb.posUltimoBloqueMB-sb.posPrimerBloqueMB;
	for (i=sb.posPrimerBloqueMB+1; i<=sb.posUltimoBloqueMB+1 ;i++){
		escribir_bit(i,1);
	}
	escribir_bit(posSB,1);
	sb.cantBloquesLibres-=(numMB+1); //Aquí también resta el sb
	bwrite(posSB,&sb);
	return 0;
}

int initAI(unsigned int inodos){ //Sobra inodos
	struct superbloque sb;
	int i,j,tamino;
	tamino = blocksize/sizeof(inodos);
	struct inodo Ainodes[tamino];//Cálculos son 7 pero según la hoja 128

	bread(posSB,&sb);
	//printf("%d \n",sb.posPrimerBloqueAI); //Muestro posición para comprobar que la lectura de sp correcta

	for (i=sb.posPrimerBloqueAI; i<=sb.posUltimoBloqueAI;i++){
		for (j=0;j<=7;j++){
			Ainodes[j].punterosDirectos[0]=i+j+1; //Enllaç a seguent inode lliure
			Ainodes[j].tipo='l'; //Tipo (libre, directorio o fichero)
		}
		if (i==sb.posUltimoBloqueAI){
			Ainodes[7].punterosDirectos[0]=9999999;
		}
		if(bwrite(i,&Ainodes)==-1){
				return -1;
			}
		}

	int numAI = sb.posUltimoBloqueAI-sb.posPrimerBloqueAI;
	for (i=sb.posPrimerBloqueAI+1; i<=sb.posUltimoBloqueAI+1 ;i++){
		escribir_bit(i,1);
	}
	sb.cantBloquesLibres-=(numAI); //Aquí también resta el sb
	bwrite(posSB,&sb);
	return 0;

}

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
	return 0;
}

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

int reservar_bloque(){
	struct superbloque sb;
	int n,pos_byte,pos_bit;
	unsigned char byte;
	unsigned char mapa_bits[blocksize];
	unsigned char bufferAux[blocksize];

	bread(posSB,&sb);		//Lectura superbloque

	if(sb.cantBloquesLibres>0){
		bread(sb.posPrimerBloqueMB,&mapa_bits);
		n = sb.posPrimerBloqueMB;
		memset (bufferAux, 255, blocksize);
		do{
			if(memcmp(mapa_bits, bufferAux, blocksize)<0){
				break;
			}else{
				n++;
				bread(n,&mapa_bits);
			}
		}while(n<=sb.posUltimoBloqueMB);

		pos_byte= n / 8;
		pos_bit= n % 8;
		byte = mapa_bits[n];

		unsigned char mascara = 128; // 10000000
		int i = 0;

		if (byte < 255) { // hay bits a 0 en el byte
			while (byte & mascara) { // operador AND para bits
				byte <<= 1; // desplazamiento de bits a la izquierda
				i++;
			}
		}
		n = ((n-sb.posPrimerBloqueMB)*blocksize+pos_byte)*8+pos_bit; //No tenemos claro n
		escribir_bit(n,1);
		sb.cantBloquesLibres-=1;
		bwrite(posSB,&sb);
		return n;
	}else{
		return -1;
	}
}

int liberar_bloque(unsigned int nbloque){
	escribir_bit(nbloque,0);
	struct superbloque sb;
	bread(posSB,&sb);		//Lectura superbloque
	sb.cantBloquesLibres+=1;
	bwrite(posSB,&sb);
	return nbloque;
}

int escribir_inodo(struct inodo inodo, unsigned int ninodo){
	struct superbloque sb;
	int ino,tamino;
	bread(posSB,&sb);		//Lectura superbloque
	ino = sb.posPrimerBloqueAI;
	tamino = blocksize/sizeof(inodo);
	ino = ninodo/tamino+ino;
	struct inodo Ainodes[tamino];
	bread(ino,&Ainodes);
	int conta = ninodo % tamino;
	Ainodes[conta]=inodo;
	bwrite(ino,&Ainodes);
	return 0;
}

struct inodo leer_inodo(unsigned int ninodo){
	struct superbloque sb;
	int ino,tamino;
	struct inodo inodo;
	bread(posSB,&sb);		//Lectura superbloque
	ino = sb.posPrimerBloqueAI;
	tamino = blocksize/sizeof(inodo);
	int conta = ninodo%tamino;
	ino = ninodo/tamino+ino;
	struct inodo Ainodes[tamino];
	bread(ino,&Ainodes);
	inodo=Ainodes[conta];
	return inodo;
}

int reservar_inodo(unsigned char tipo, unsigned char permisos){
	struct superbloque sb;
	int illiure,i,tamino,ino,nbloque;
	bread(posSB,&sb);		//Lectura superbloque
	illiure = sb.posPrimerInodoLibre; //Debe pasar a bloque
	ino = sb.posPrimerBloqueAI;
	struct inodo in;
	tamino = blocksize/sizeof(in);
	nbloque = illiure/tamino+ino;

	in.tipo=tipo;
	in.permisos=permisos;
	in.nlinks=1;
	in.tamEnBytesLog=0;
	in.atime=time(NULL);
	in.ctime=time(NULL);
	in.mtime=time(NULL);
	in.numBloquesOcupados=0;
	for(i=0;i<=11;i++){
		in.punterosDirectos[i]=0;
	}
	for(i=0;i<2;i++){
		in.punterosIndirectos[i]=0;
	}

	struct inodo Ainodes[tamino];
	int conta = ino%tamino;
	sb.posPrimerInodoLibre=Ainodes[conta].punterosDirectos[0];
	sb.cantInodosLibres--;
	bread(nbloque,&Ainodes);
	Ainodes[conta]=in;
	bwrite(nbloque,&Ainodes);
	bwrite(posSB,&sb);
	return ino;
}

int traducir_bloque_inodo(unsigned int ninodo, unsigned int blogico, unsigned int *bfisico, char reservar){
	int pdir,npun,pind1,pind2,ino,nbloque,i;
	struct inodo in;


	pdir = 12;
	npun = 256;
	int bufN0[npun];
	pind1 = npun*npun;
	pind2 = npun*pind1;

	in = leer_inodo(ninodo);

	if (blogico < pdir){
		switch (reservar){
		case '0':
			if (in.punterosDirectos[blogico]==0){
				return -1;
			}else{
				*bfisico = in.punterosDirectos[blogico];
				return 0;
			}
			break;
		case '1':
			if (in.punterosDirectos[blogico]==0){
				nbloque = reservar_bloque();
				if (nbloque<-1){
					in.punterosDirectos[blogico]=nbloque;
					in.numBloquesOcupados++;
					*bfisico=in.punterosDirectos[blogico];
					escribir_inodo(in,ninodo);
					return 0;
				}else{
					*bfisico = in.punterosDirectos[blogico];
					return 0;
				}
			}
			break;
		default:
			return -2;
			break;
		}
	}else if(blogico<(pdir+npun)){
		switch(reservar){
		case '0':
			if (in.punterosIndirectos[0] == 0){
				return -1;
			}else{
				if (leer_inodo(in.punterosIndirectos[0]-pdir)==0){
					return -1;
				}else{
					in = leer_inodo(in.punterosIndirectos[0]-pdir);
					for (i=0;i<npun;i++){

						bufN0[i]=
					}
					*bfisico = in.punterosDirectos[blogico-pdir]; //Mirar
					return 0;
				}

			}
			break;
		}
	}


}





