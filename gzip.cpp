/*Author: Rui Pedro Paiva
Teoria da Informa��o, LEI, 2007/2008*/

#include <cstdlib>
#include "huffman.h"
#include "gzip.h"
#include <stddef.h>

//fun��o principal, a quhal gere todo o processo de descompacta��o
int main(int argc, char** argv)
{
	//--- Gzip file management variables
	FILE *gzFile;  //ponteiro para o ficheiro a abrir
	long fileSize;
	long origFileSize;
	int numBlocks = 0;
	gzipHeader gzh;
	unsigned char byte;  //vari�vel tempor�ria para armazenar um byte lido directamente do ficheiro
	unsigned int rb = 0;  //�ltimo byte lido (poder� ter mais que 8 bits, se tiverem sobrado alguns de leituras anteriores)
	char needBits = 0;
	char availBits = 0;
	int hclenIterator;



	//--- obter ficheiro a descompactar
	char fileName[] = "FAQ.txt.gz";

	/*if (argc != 2)
	{
		printf("Linha de comando invalida!!!");
		return -1;
	}*/
	//char * fileName = argv[1];

	//--- processar ficheiro
	gzFile = fopen(fileName, "r");
	fseek(gzFile, 0L, SEEK_END);
	fileSize = ftell(gzFile);
	fseek(gzFile, 0L, SEEK_SET);

	//ler tamanho do ficheiro original (acrescentar: e definir Vector com s�mbolos
	origFileSize = getOrigFileSize(gzFile);


	//--- ler cabe�alho
	int erro = getHeader(gzFile, &gzh);
	if (erro != 0)
	{
		printf ("Formato invalido!!!");
		return -1;
	}

	//--- Para todos os blocos encontrados
	char BFINAL;
	int HLIT,HDIST,HCLEN;
	do
	{
		//--- ler o block header: primeiro byte depois do cabe�alho do ficheiro
		needBits = 3;
		if (availBits < needBits)
		{
			fread(&byte, 1, 1, gzFile);
			rb = (byte << availBits) | rb;
			availBits += 8;
		}
		//obter BFINAL
		//ver se � o �ltimo bloco
		BFINAL = rb & 0x01; //primeiro bit � o menos significativo
		printf("BFINAL = %d\n", BFINAL);
		rb = rb >> 1; //descartar o bit correspondente ao BFINAL
		availBits -=1;

		//analisar block header e ver se � huffman din�mico
		if (!isDynamicHuffman(rb))  //ignorar bloco se n�o for Huffman din�mico
			continue;
		rb = rb >> 2; //descartar os 2 bits correspondentes ao BTYPE
		availBits -= 2;

		//Ex1
		HLIT=lerBloco(5,&availBits,&rb,gzFile);
		printf("HLIT = %d\n",HLIT);
		HDIST=lerBloco(5,&availBits,&rb,gzFile);
		printf("HDIST = %d\n",HDIST);
		HCLEN=lerBloco(4,&availBits,&rb,gzFile);
		printf("HCLEN = %d\n",HCLEN);

        //Ex2
        int ordemSequencias[]={16,17,18,0,8,7,9,6,10,5,11,4,12,3,13,2,14,1,15};
        int compCodgAlfCompCodg[19];
        int zeros;
        for(zeros=0;zeros<19;zeros++){
            compCodgAlfCompCodg[zeros]=0;
        }
        int ex2Iterator;

        printf("Array dos comprimentos dos codigos do alfabeto de comprimentos de codigos: \n");
		for(hclenIterator=0;hclenIterator<(HCLEN+4);hclenIterator++){
            ex2Iterator = lerBloco(3,&availBits,&rb,gzFile);
            compCodgAlfCompCodg[ordemSequencias[hclenIterator]]=ex2Iterator;

		}
		//imprime
        for(hclenIterator=0;hclenIterator<19;hclenIterator++){
            printf("%d: %d\n",hclenIterator,compCodgAlfCompCodg[hclenIterator]);
		}

		//ex3
		int codHuffman[19];
		for(zeros=0;zeros<19;zeros++){
            codHuffman[zeros]=-1;
        }
		 printf("Distancias: \n");
		 int maximun;
		 maximun = findMaxBits(compCodgAlfCompCodg);
		 criaCodigosDeHuffman(maximun,codHuffman,compCodgAlfCompCodg);

		 //print distancias
        for(hclenIterator=0;hclenIterator<19;hclenIterator++){
            printf("%d: %d\n",hclenIterator,codHuffman[hclenIterator]);
		}

        char codHuffman2[19][5];
		for(zeros=0;zeros<19;zeros++){
                for(int h=0;h<5;h++){
                     codHuffman2[zeros][h]='\0';
                }

        }

        //converter int to binario
        getBin(compCodgAlfCompCodg,codHuffman, codHuffman2);


		for(zeros=0;zeros<19;zeros++){
                printf("%d: ",zeros);
                for(int h=0;h<5;h++){
                     printf("%c",codHuffman2[zeros][h]);
                }
                printf("\n ");

        }
        //criar arvore huffma
        printf("A criar arvore e adicionar codigos...\n");
        HuffmanTree* ArvoreHuffman = createHFTree();
        addCodesToTree(ArvoreHuffman,19,compCodgAlfCompCodg,codHuffman);
        //exercicio 4
        int arrayHLIT[HLIT+257];

        for(zeros=0;zeros<HLIT+257;zeros++){
            arrayHLIT[zeros]=0;
        }
        readHuffmanTree(ArvoreHuffman,HLIT+257,arrayHLIT,&availBits,&rb,gzFile);

        //Imprir arrayHLIT
        printf("Array HLIT:\n");
        for(zeros=0;zeros<HLIT+257;zeros++){
            printf("%d: %d\n",zeros,arrayHLIT[zeros]);
        }

        //Exercicio 5
        int arrayHDIST[HDIST+1];
        for(zeros=0;zeros<HDIST+1;zeros++){
            arrayHDIST[zeros]=0;
        }
        readHuffmanTree(ArvoreHuffman,HDIST+1,arrayHDIST,&availBits,&rb,gzFile);
        //Imprimir arrayHDIST
        printf("Array HDIST:\n");
        for(zeros=0;zeros<HDIST+1;zeros++){
            printf("%d: %d\n",zeros,arrayHDIST[zeros]);
        }
        //Exercicio 6


		//actualizar n�mero de blocos analisados
		numBlocks++;
	}while(BFINAL == 0);


	//termina��es
	fclose(gzFile);
	printf("End: %d bloco(s) analisado(s).\n", numBlocks);

    //RETIRAR antes de criar o execut�vel final
    system("PAUSE");
    return EXIT_SUCCESS;
}
void readHuffmanTree(HuffmanTree* ArvoreHuffman,int Nelementos,int* arrayHLIT,char* availBits,unsigned int* rb,FILE* gzFile){
    int i,j,bit,nextCode;

    for(i=0; i<Nelementos;i++){
        resetCurNode(ArvoreHuffman); // Voltar a por o ponteiro na raiz da arvore
        do{
            bit=lerBloco(1,availBits,rb,gzFile);
            nextCode= nextNode(ArvoreHuffman,bit+48);
        }while(nextCode<0);
        if(nextCode<16){
            arrayHLIT[i]=nextCode;
        }
        if(nextCode==16){
            bit=lerBloco(2,availBits,rb,gzFile)+3; // dar "shift" no intervalo
            for(j=0;j<bit-1;j++){
                arrayHLIT[i+j]=arrayHLIT[i-1];
            }
            i+=j;
        }
        if(nextCode==17){
            bit=lerBloco(3,availBits,rb,gzFile)+3;
            i+=bit-1; //Como inicializei o array a 0's basta incrementar no iterador

        }
        if(nextCode==18){
            bit=lerBloco(7,availBits,rb,gzFile)+11;
            i+= bit-1; //Como inicializei o array a 0's basta incrementar no iterador
        }
    }

}
void addCodesToTree(HuffmanTree *ArvoreHuffman, int n, int* comprimentosAlf, int* codHuffman ){
   int i;
   //char str[5];
   for (i=0;i<n;i++){
        if(comprimentosAlf[i]>0){
            char str[8];
            codeToString(str,codHuffman[i],comprimentosAlf[i]);
            addNode(ArvoreHuffman,str,i,1);
        }
   }
}
void getBin(int* compCodgAlfCompCodg,int* codHuffman,  char codHuffman2[][5]){
    int i = 0;
    while(i<19){
        //printf("entrou\n");
        if(codHuffman[i]!=-1){
           bitToString(compCodgAlfCompCodg[i],codHuffman[i],codHuffman2[i]);
        }
        i++;
    }

}
void codeToString(char strBits[], int num ,int distancia){

	char mask = 0x01;  //get LSbit

	for (char bit, i = distancia-1; i >= 0; i--)
	{
		bit = num & mask;
		strBits[i] = bit +48; //converter valor num�rico para o caracter alfanum�rico correspondente
		num = num >> 1;
	}
	strBits[distancia]='\0';
	printf("grifo: %s\n",strBits);

}
void bitToString(int compCodgAlfCompCodg,int codHuffman, char* codHuffman2){
    int i;
    char bit=0;
    //unsigned char output[compCodgAlfCompCodg];
    codHuffman2[compCodgAlfCompCodg]='\0';
    for(i=compCodgAlfCompCodg-1;i>=0;i--){
        bit=codHuffman%2;
        codHuffman2[i]=(bit&1)+48;
        codHuffman = codHuffman/2;
    }


}

void criaCodigosDeHuffman(int maxBits,int* codHuffman,int* compCodgAlfCompCodg){
    unsigned char code;
    int bits;
    unsigned int bl_count[maxBits];
    unsigned char next_code[maxBits];
    code = 0;
    bl_count[0] = 0;
    for(int j=1; j<=maxBits;j++){
        bl_count[j] = 0;
        next_code[j]=0;
    }
    next_code[0] = 0;
    for(int j=1; j<=maxBits;j++){
        for(int i=0; i<=19;i++){
            if(compCodgAlfCompCodg[i]==j){
                bl_count[j]+=1;
            }
        }
    }
    for (bits = 1; bits <= maxBits; bits++) {
        code = (code + bl_count[bits-1]) << 1;
        next_code[bits] = code;
    }
    int count =0;
    int codigo=0;
    for(int count=0;count<5;count++){
        codigo=next_code[count+1];
        for(int i=0;i<19;i++){
            if(compCodgAlfCompCodg[i]==count+1){
                codHuffman[i]=codigo;
                codigo=codigo+1;
            }
        }
    }
}

int lerBloco(int needBits,char* availBits,unsigned int* rb,FILE* gzFile){
    int result;
    unsigned char mascara;
    unsigned char byte;
    if (*availBits < needBits){
        fread(&byte, 1, 1, gzFile);
        *rb = (byte << (*availBits)) | *rb;
        *availBits += 8;
    }

    mascara = (1 << needBits) - 1;
    result = (*rb) & mascara;
    *rb = *rb >> needBits;
    *availBits -=needBits;

    return result;
}


//---------------------------------------------------------------
//L� o cabe�alho do ficheiro gzip: devolve erro (-1) se o formato for inv�lidodevolve, ou 0 se ok
int getHeader(FILE *gzFile, gzipHeader *gzh) //obt�m cabe�alho
{
	unsigned char byte;

	//Identica��o 1 e 2: valores fixos
	fread(&byte, 1, 1, gzFile);
	(*gzh).ID1 = byte;
	if ((*gzh).ID1 != 0x1f) return -1; //erro no cabe�alho

	fread(&byte, 1, 1, gzFile);
	(*gzh).ID2 = byte;
	if ((*gzh).ID2 != 0x8b) return -1; //erro no cabe�alho

	//M�todo de compress�o (deve ser 8 para denotar o deflate)
	fread(&byte, 1, 1, gzFile);
	(*gzh).CM = byte;
	if ((*gzh).CM != 0x08) return -1; //erro no cabe�alho

	//Flags
	fread(&byte, 1, 1, gzFile);
	unsigned char FLG = byte;

	//MTIME
	char lenMTIME = 4;
	fread(&byte, 1, 1, gzFile);
	(*gzh).MTIME = byte;
	for (int i = 1; i <= lenMTIME - 1; i++)
	{
		fread(&byte, 1, 1, gzFile);
		(*gzh).MTIME = (byte << 8) + (*gzh).MTIME;
	}

	//XFL (not processed...)
	fread(&byte, 1, 1, gzFile);
	(*gzh).XFL = byte;

	//OS (not processed...)
	fread(&byte, 1, 1, gzFile);
	(*gzh).OS = byte;

	//--- Check Flags
	(*gzh).FLG_FTEXT = (char)(FLG & 0x01);
	(*gzh).FLG_FHCRC = (char)((FLG & 0x02) >> 1);
	(*gzh).FLG_FEXTRA = (char)((FLG & 0x04) >> 2);
	(*gzh).FLG_FNAME = (char)((FLG & 0x08) >> 3);
	(*gzh).FLG_FCOMMENT = (char)((FLG & 0x10) >> 4);

	//FLG_EXTRA
	if ((*gzh).FLG_FEXTRA == 1)
	{
		//ler 2 bytes XLEN + XLEN bytes de extra field
		//1� byte: LSB, 2�: MSB
		char lenXLEN = 2;

		fread(&byte, 1, 1, gzFile);
		(*gzh).xlen = byte;
		fread(&byte, 1, 1, gzFile);
		(*gzh).xlen = (byte << 8) + (*gzh).xlen;

		(*gzh).extraField = new unsigned char[(*gzh).xlen];

		//ler extra field (deixado como est�, i.e., n�o processado...)
		for (int i = 0; i <= (*gzh).xlen - 1; i++)
		{
			fread(&byte, 1, 1, gzFile);
			(*gzh).extraField[i] = byte;
		}
	}
	else
	{
		(*gzh).xlen = 0;
		(*gzh).extraField = 0;
	}

	//FLG_FNAME: ler nome original
	if ((*gzh).FLG_FNAME == 1)
	{
		(*gzh).fName = new char[1024];
		unsigned int i = 0;
		do
		{
			fread(&byte, 1, 1, gzFile);
			if (i <= 1023)  //guarda no m�ximo 1024 caracteres no array
				(*gzh).fName[i] = byte;
			i++;
		}while(byte != 0);
		if (i > 1023)
			(*gzh).fName[1023] = 0;  //apesar de nome incompleto, garantir que o array termina em 0
	}
	else
		(*gzh).fName = 0;

	//FLG_FCOMMENT: ler coment�rio
	if ((*gzh).FLG_FCOMMENT == 1)
	{
		(*gzh).fComment = new char[1024];
		unsigned int i = 0;
		do
		{
			fread(&byte, 1, 1, gzFile);
			if (i <= 1023)  //guarda no m�ximo 1024 caracteres no array
				(*gzh).fComment[i] = byte;
			i++;
		}while(byte != 0);
		if (i > 1023)
			(*gzh).fComment[1023] = 0;  //apesar de coment�rio incompleto, garantir que o array termina em 0
	}
	else
		(*gzh).fComment = 0;


	//FLG_FHCRC (not processed...)
	if ((*gzh).FLG_FHCRC == 1)
	{
		(*gzh).HCRC = new unsigned char[2];
		fread(&byte, 1, 1, gzFile);
		(*gzh).HCRC[0] = byte;
		fread(&byte, 1, 1, gzFile);
		(*gzh).HCRC[1] = byte;
	}
	else
		(*gzh).HCRC = 0;

	return 0;
}


//---------------------------------------------------------------
//Analisa block header e v� se � huffman din�mico
int isDynamicHuffman(unsigned char rb)
{
	unsigned char BTYPE = rb & 0x03;

	if (BTYPE == 0) //--> sem compress�o
	{
		printf("Ignorando bloco: sem compacta��o!!!\n");
		return 0;
	}
	else if (BTYPE == 1)
	{
		printf("Ignorando bloco: compactado com Huffman fixo!!!\n");
		return 0;
	}
	else if (BTYPE == 3)
	{
		printf("Ignorando bloco: BTYPE = reservado!!!\n");
		return 0;
	}
	else
		return 1;
}


//---------------------------------------------------------------
//Obt�m tamanho do ficheiro original
long getOrigFileSize(FILE * gzFile)
{
	//salvaguarda posi��o actual do ficheiro
	long fp = ftell(gzFile);

	//�ltimos 4 bytes = ISIZE;
	fseek(gzFile, -4, SEEK_END);

	//determina ISIZE (s� correcto se cabe em 32 bits)
	unsigned long sz = 0;
	unsigned char byte;
	fread(&byte, 1, 1, gzFile);
	sz = byte;
	for (int i = 0; i <= 2; i++)
	{
		fread(&byte, 1, 1, gzFile);
		sz = (byte << 8*(i+1)) + sz;
	}


	//restaura file pointer
	fseek(gzFile, fp, SEEK_SET);

	return sz;
}


//---------------------------------------------------------------
void bits2String(char *strBits, unsigned char byte)
{
	char mask = 0x01;  //get LSbit

	strBits[8] = 0;
	for (char bit, i = 7; i >= 0; i--)
	{
		bit = byte & mask;
		strBits[i] = bit +48; //converter valor num�rico para o caracter alfanum�rico correspondente
		byte = byte >> 1;
	}
}
void bits2String2(char *strBits, int bits,int num)
{
	char mask = 0x01;  //get LSbit

	for (char bit, i = bits-1; i >= 0; i--)
	{
		bit = num & mask;
		strBits[i] = bit +48; //converter valor num�rico para o caracter alfanum�rico correspondente
		num = num >> 1;
	}
	strBits[bits]='\0';
}

/*void getBin(int num,unsigned char *str){
  *(str+5) = '\0';
  int mask = 0x10 << 1;
  while(mask >>= 1)
    *str++ = !!(mask & num) + '0';
}*/

int findMaxBits(int* compCodgAlfCompCodg){
    int maximum,c;
    maximum=0;
    c=0;
    for (c = 1; c <= (int)sizeof(compCodgAlfCompCodg); c++){
        if (compCodgAlfCompCodg[c] > maximum){
            maximum  = compCodgAlfCompCodg[c];
        }
    }
    return maximum;
}
