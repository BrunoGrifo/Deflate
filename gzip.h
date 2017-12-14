/* Author: Rui Pedro Paiva
Teoria da Informação, LEI, 2007/2008*/

#include <stdio.h>

// Gzip header variables
typedef struct header
{
	//elementos fixos
	unsigned char ID1, ID2, CM, XFL, OS;
	unsigned long MTIME;
	unsigned char FLG_FTEXT, FLG_FHCRC, FLG_FEXTRA, FLG_FNAME, FLG_FCOMMENT;   //bits 0, 1, 2, 3 e 4, respectivamente (restantes 3: reservados)

	// FLG_FTEXT --> ignorado deliberadamente (tipicamente igual a 0)
	//se FLG_FEXTRA == 1
	unsigned char xlen;
	unsigned char *extraField;

	//se FLG_FNAME == 1
	char *fName;  //terminada por um byte a 0

	//se FLG_FCOMMENT == 1
	char *fComment; //terminada por um byte a 0

	//se FLG_HCRC == 1
	unsigned char *HCRC;
} gzipHeader;


typedef struct lnode *Simbolo;

typedef struct lnode {
    char caracter;
    Simbolo next;
} Simbolo_node;

void insere_lista (Simbolo lista, char it);
Simbolo cria_lista (void);

long getOrigFileSize(FILE *gzFile);
int getHeader(FILE *gzFile, gzipHeader *gzh);
int isDynamicHuffman(unsigned char rb);
void bits2String(char *strBits, unsigned char byte);
//My functions
int lerBloco(int needBits,char *availBits,unsigned int *rb,FILE *gzFile);
int findMaxBits(int *compCodgAlfCompCodg);
//void getBin(int num,unsigned char *str);
void getBin(int* compCodgAlfCompCodg,unsigned int* codHuffman,  char codHuffman2[][5]);
void bitToString(int compCodgAlfCompCodg,int codHuffman, char* codHuffman2);
void criaCodigosDeHuffman(int maxBits,int iteracoes, unsigned int* codHuffman,int* compCodgAlfCompCodg);

void addCodesToTree(HuffmanTree *ArvoreHuffman, int n, int* comprimentosAlf,unsigned int* codHuffman );
void codeToString(char str[], int compAlf,int codHuffman);
void readHuffmanTree(HuffmanTree *ArvoreHuffman,int Nelementos,int* arrayHLIT,char* availBits,unsigned int* rb,FILE* gzFile);
