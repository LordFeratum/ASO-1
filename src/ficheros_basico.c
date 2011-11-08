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
	int i,j,tamino, x;
	tamino = blocksize/T_INODO;
	struct inodo Ainodes[tamino];//Cálculos son 7 pero según la hoja 128

	bread(posSB,&sb);
	//printf("%d \n",sb.posPrimerBloqueAI); //Muestro posición para comprobar que la lectura de sp correcta
	x = 1;
	for (i=sb.posPrimerBloqueAI; i<=sb.posUltimoBloqueAI;i++){
		for (j=0;j<=blocksize/tamino;j++){
			Ainodes[j].punterosDirectos[0]=x; //Enllaç a seguent inode lliure
			Ainodes[j].tipo='l'; //Tipo (libre, directorio o fichero)
			x++;
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


		byte = mapa_bits[n];

		unsigned char mascara = 128; // 10000000
		int i = 0;

		if (byte < 255) { // hay bits a 0 en el byte
			while (byte & mascara) { // operador AND para bits
				byte <<= 1; // desplazamiento de bits a la izquierda
				pos_bit++;
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
	tamino = blocksize/T_INODO;
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
	tamino = blocksize/T_INODO;
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
	tamino = blocksize/T_INODO;
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
	ino=sb.posPrimerInodoLibre;
	sb.posPrimerInodoLibre=Ainodes[conta].punterosDirectos[0];
	sb.cantInodosLibres--;
	bread(nbloque,&Ainodes);
	Ainodes[conta]=in;
	escribir_inodo(in,ino);
	bwrite(posSB,&sb);
	return ino;
}

int traducir_bloque_inodo(unsigned int ninodo, unsigned int blogico, unsigned int *bfisico, char reservar){
	int pdir,npun,pind1,pind2,nbloque,i,punt0,punt1,punt2;
	struct inodo in;

	pdir = 12;
	npun = 256;
	int bufferIndirectos0[npun];
	int bufferIndirectos1[npun];
	int bufferIndirectos2[npun];
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
				if (bread(in.punterosIndirectos[0],bufferIndirectos0)){
					return -1;
				}else{
					return bufferIndirectos0[blogico-npun];
				}
			}
			break;
		case '1':
			if (in.punterosIndirectos[0]==0){
				bufferIndirectos0[blogico-pdir] = reservar_bloque();
				in.numBloquesOcupados++;
				in.punterosIndirectos[0]= reservar_bloque();
				for (i=0; i<npun;i++){
					bufferIndirectos0[i]=0;
				}
				bwrite(in.punterosIndirectos[0],bufferIndirectos0);
			}else{
				bread(in.punterosIndirectos[0],bufferIndirectos0);
				if(bufferIndirectos0[blogico]==0){
					bufferIndirectos0[blogico-pdir] = reservar_bloque();
					in.numBloquesOcupados++;
					bwrite(in.punterosIndirectos[0],bufferIndirectos0);
					return bufferIndirectos0[blogico-pdir];
				}else{
					return bufferIndirectos0[blogico-pdir];
				}
			}
			escribir_inodo(in,ninodo);
			break;
		default:
			return -2;
			break;
		}
	}else if (blogico < pdir+npun+pind1){
		punt0 = (blogico -(pdir+npun)) %npun;
		punt1 = (blogico -(pdir+npun)) /npun;
		switch(reservar){
		case '0'://query mode
			if(in.punterosIndirectos[1] == 0){
				return -1;
			} else {
				bread(in.punterosIndirectos[1],bufferIndirectos1);
				if (bufferIndirectos1[punt1]==0){
					return -4;
				}else{
					bread(in.punterosIndirectos[0],bufferIndirectos0);
					if (bufferIndirectos0[punt0]==0){
						return -1;
					}else{
						*bfisico = bufferIndirectos0[punt0];
						return 0;
					}
				}
			}
			break;
		case '1'://write mode
			if (in.punterosIndirectos[1]==0){
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
				if (bufferIndirectos0[punt0]==0){
					return -1;
				}else{
					*bfisico = bufferIndirectos0[punt0];
					return 0;
				}
			}else{
				bread(in.punterosIndirectos[1],bufferIndirectos1);
				bread(bufferIndirectos1[punt1],bufferIndirectos0);
				if (bufferIndirectos0[punt0]==0){
					bufferIndirectos0[punt0] = reservar_bloque();
					in.numBloquesOcupados++;
					bwrite(bufferIndirectos1[punt1], bufferIndirectos0);
					*bfisico = bufferIndirectos0[punt0];
					return 0;
				}else{
					*bfisico = bufferIndirectos0[punt0];
					return 0;
				}
			}
			break;
		}
		escribir_inodo(in,ninodo);
	}else if(blogico < pdir+npun+pind1+pind2){
		punt2 = (blogico - (pdir+npun+pind1))/pind1;
		punt1 = ((blogico -(pdir+npun+pind1)) %pind1)/npun;
		punt0 = ((blogico -(pdir+npun+pind1))% pind1)%npun;
		switch(reservar){
		case '0':
			if(in.punterosIndirectos[2] == 0){
				return -1;
			}else{
				bread(in.punterosIndirectos[2],bufferIndirectos2);
				if (bufferIndirectos2[punt2]==0){
					return -1;
				}else{
					bread(bufferIndirectos2[punt2],bufferIndirectos1);
					if (bufferIndirectos1[punt1]==0){
						return -1;
					}else{
						bread(bufferIndirectos1[punt1],bufferIndirectos0);
						if (bufferIndirectos0[punt0]==0){
							return -1;
						}else{
							*bfisico = bufferIndirectos0[punt0];
							return 0;
						}
					}
				}
			}
		break;
		case '1':
			if(in.punterosIndirectos[2] == 0){
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
				*bfisico = bufferIndirectos0[punt0];
				return 0;
			}else{
				bread(in.punterosIndirectos[2]-pdir,bufferIndirectos2);
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
					*bfisico = bufferIndirectos0[punt0];
					return 0;
				}else{
					bufferIndirectos2[punt2] = reservar_bloque();
					if (bufferIndirectos2[punt2]== 0){
						bufferIndirectos1[punt1] = reservar_bloque();
						for (i=0; i<npun;i++){
							bufferIndirectos0[i]=0;
						}
						bufferIndirectos0[punt0] = reservar_bloque();
						in.numBloquesOcupados++;
						bwrite(bufferIndirectos2[punt2], bufferIndirectos1);
						bwrite(bufferIndirectos1[punt1], bufferIndirectos0);
						*bfisico = bufferIndirectos0[punt0];
						return 0;
					}else{
						bufferIndirectos1[punt1] = reservar_bloque();
						if (bufferIndirectos0[punt0] == 0){
							bufferIndirectos0[punt0] = reservar_bloque();
							in.numBloquesOcupados++;
							bwrite(bufferIndirectos1[punt1], bufferIndirectos0);
							*bfisico = bufferIndirectos0[punt0];
							return 0;
						}else{
							*bfisico = bufferIndirectos0[punt0];
							return 0;
						}
					}
				}
			}
		break;
		}
		escribir_inodo(in,ninodo);
		return 0;
	}
}

int liberar_inodo(unsigned int ninodo){
	int ino;
	struct superbloque sb;
	struct inodo in;

	bread(posSB,&sb);
	in = leer_inodo(ninodo);
	liberar_bloques_inodo(ninodo,0);
	in.tipo='l';
	in.punterosDirectos[0]=sb.posPrimerInodoLibre;
	sb.posPrimerInodoLibre=ninodo;
	sb.cantInodosLibres++;
	escribir_inodo(in,ninodo);
	bwrite(posSB,&sb);
	return ninodo;
}

int liberar_bloques_inodo(unsigned int ninodo, unsigned int blogico){
	struct inodo in;

	int zero = 0;
	int i,blog,punt0,punt1,punt2;
	int n = 0;
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

	for (i=0; i<num_punteros;i++){
		bufferIndirectos[i]=0;
	}

	for (blog=blogico;blog <= ultimoBloque; blog++){
		if (blog< num_pDirectos){
			if (in.punterosDirectos[blog] > 0){
				liberar_bloque(blog);
				in.punterosDirectos[blog] = 0;
				in.numBloquesOcupados--;
			}
		}
		else if(blog < num_pDirectos + num_punteros){
			if (in.punterosIndirectos[0]==0){
				return -1;
			}else{
				bread(in.punterosIndirectos[0],bufferIndirectos0);
				if (bufferIndirectos0[blog-num_pDirectos]==0){
					return -1;
				}else{
					liberar_bloque(bufferIndirectos0[blog-num_pDirectos]);
					bufferIndirectos0[blog-num_pDirectos]=0;
					in.numBloquesOcupados--;
					if (memcmp(bufferIndirectos0, bufferIndirectos, num_punteros)==0){
						liberar_bloque(in.punterosIndirectos[0]);
						in.punterosIndirectos[0]=0;
					}else{
						bwrite(in.punterosIndirectos[0],bufferIndirectos0);
					}
				}
			}
		}
		else if(blog<num_pDirectos+num_punteros+num_punteros2){
			punt0 = (blog - (num_pDirectos + num_punteros)) % num_punteros;
			punt1 = (blog - (num_pDirectos + num_punteros)) / num_punteros;
			if (in.punterosIndirectos[1]==0){
				return -1;
			}else{
				bread(in.punterosIndirectos[1],bufferIndirectos1);
				if (bufferIndirectos1[punt1]==0){
					return -1;
				}else{
					bread(bufferIndirectos1[punt1],bufferIndirectos0);
					if (bufferIndirectos0[punt0]==0){
						return -1;
					}else{
						liberar_bloque(bufferIndirectos0[punt0]);
						bufferIndirectos0[punt0]=0;
						in.numBloquesOcupados--;
						if (memcmp(bufferIndirectos0, bufferIndirectos, num_punteros)==0){
							liberar_bloque(bufferIndirectos1[punt1]);
							bufferIndirectos1[punt1]=0;
						}else{
							bwrite(bufferIndirectos1[punt1],bufferIndirectos0);
						}
						if (memcmp(bufferIndirectos1, bufferIndirectos, num_punteros)==0){
							liberar_bloque(in.punterosIndirectos[1]);
							in.punterosIndirectos[1]=0;
						}else{
							bwrite(in.punterosIndirectos[1],bufferIndirectos1);
						}
					}
				}
			}
		}
		else if(blog < num_pDirectos + num_punteros + num_punteros2 + num_punteros3){
			punt0 = ((blog - (num_pDirectos + num_punteros + num_punteros2)) % num_punteros2) % num_punteros;
			punt1 = ((blog - (num_pDirectos + num_punteros + num_punteros2)) % num_punteros2) / num_punteros;
			punt2 = (blog - (num_pDirectos + num_punteros + num_punteros2)) / num_punteros2;
			if (in.punterosIndirectos[2]==0){
				return -1;
			}else{
				bread(in.punterosIndirectos[2],bufferIndirectos2);
				if (bufferIndirectos2[punt2]==0){
					return -1;
				}else{
					bread(bufferIndirectos2[punt2],bufferIndirectos1);
					if (bufferIndirectos1[punt1]==0){
						return -1;
					}else{
						bread(bufferIndirectos1[punt1],bufferIndirectos0);
						if (bufferIndirectos0[punt0]==0){
							return -1;
						}else{
							liberar_bloque(bufferIndirectos0[punt0]);
							bufferIndirectos0[punt0]=0;
							in.numBloquesOcupados--;
							if (memcmp(bufferIndirectos0, bufferIndirectos, num_punteros)==0){
								liberar_bloque(bufferIndirectos1[punt1]);
								bufferIndirectos1[punt1]=0;
							}else{
								bwrite(bufferIndirectos1[punt1],bufferIndirectos0);
							}
							if (memcmp(bufferIndirectos1, bufferIndirectos, num_punteros)==0){
								liberar_bloque(bufferIndirectos2[punt2]);
								bufferIndirectos2[punt2]=0;
							}else{
								bwrite(bufferIndirectos2[punt2],bufferIndirectos1);
							}
							if (memcmp(bufferIndirectos2, bufferIndirectos, num_punteros)==0){
								liberar_bloque(in.punterosIndirectos[2]);
								in.punterosIndirectos[2]=0;
							}else{
								bwrite(in.punterosIndirectos[2],bufferIndirectos2);
							}
						}
					}
				}
			}
		}
	}
	escribir_inodo(in,ninodo);
}





