/*******************************************************************************
* 																			   *
*						SYSTEM PROGRAMMING									   *
*					HW01 141044042 RABİA ÇAKAS								   *
*	main.c					 			 									   *
*---------																	   *
*	Created on 01/03/2017 by Rabia Çakas									   *
*																			   *
*******************************************************************************/

/*******************************************************************************
*					Includes												   *
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/*******************************************************************************
*					Function Prototypes										   *
*******************************************************************************/

int findInFile(FILE *inputP, const char* fileName, const char* word);

/*******************************************************************************
* 	int findInFile(FILE *inputP, const char* fileName, const char* word);	   *
*------------------------------------------------------------------------------*
* Bu fonksiyonda arguman olarak gelen dosyadaki karakterlerin sayisi bulunur,  *
* bir array e atilir, aranacak string in boyutunda yeni bir array olusturulur  *
* gelen karaktere gore row ve col hesaplanir,eslesme gordugu durumda row ve col*
* print edilir, fonksiyon eslesme sayisini return eder.						   *
*******************************************************************************/

int main(int argc, char* argv[])
{
	int countOfWord=0;
	FILE * inputP;
	if(argc!= 3){

		fprintf(stderr, "The usage %s string can be that form : ./list string fileName.", argv[0]);
		return -1;

	}
	else if(strcmp(argv[0], "./list")!= 0){

		fprintf(stderr, "ARGUMENT ERROR: %s\n", argv[0]);
		return -1;
	}
	else if((inputP= fopen(argv[2], "r"))== NULL){

		fprintf(stderr, "OPENING ERROR: %s > %s\n", argv[2], strerror(errno));
		return -1;	
	}
	else{
		countOfWord=findInFile(inputP,argv[2], argv[1]);
		printf("\n%d adet %s bulundu\n", countOfWord, argv[1]);
	}
	return 0;
}

int findInFile(FILE *inputP, const char* fileName, const char* word)
{
	char c;
	int size = 0, newLineCounter = 1, maxLineLength = 0, max = 0,i, column = 1,
	row = 1, countOfword = 0, j, sizeofWord,k;
	char * totalArray=NULL;
	char *word1=NULL;

	sizeofWord = strlen(word);
	word1=(char *)calloc(sizeofWord+1,sizeof(char));;
	
	c = fgetc(inputP);
	max++;
	while(c != EOF)
	{
		size++;
		if(c == '\n'){
			newLineCounter++;
			max = 0;
		}
		if(max > maxLineLength)
			maxLineLength = max;
		c = fgetc(inputP);
		max++;
	}
	rewind(inputP);

	totalArray=(char *)calloc(size+2, sizeof(char));;
	
	c=fgetc(inputP);
	for(i=0; i<size && c!=EOF; i++){
		totalArray[i]=c;
		c=fgetc(inputP);
	}
	fclose(inputP);

	for (i = 0; i < size; ++i)
	{
		if(totalArray[i] == ' ' || totalArray[i] == '\t')
			column++;
		else if(totalArray[i] == '\n'){
			row ++;
			column = 1;
		}
		else
		{
			word1[0] = totalArray[i];
			j = 1;
            k = i;
            k++;
			while(j < sizeofWord)
			{
				if(totalArray[k] != '\n'&& totalArray[k] != '\t' && totalArray[k] != ' ')
                {
               		 word1[j] = totalArray[k];
              		 j++;
					 k++;
                                    
				}
				
				else if(totalArray[k] == '\n'){
					k++;
				}
				else{
					k++;
				}
				
			}
			if (strcmp(word1, word) == 0){
				printf("[%d,%d] konumunda ilk karakter bulundu\n",row, column);
				countOfword++;
			}
			column++;
		}
	}
	totalArray[size]='\0';
	if(totalArray){
		free(totalArray);}
	if(word1){
		free(word1);
	}
	
	totalArray=NULL;
	word1=NULL;
	return countOfword;
}
