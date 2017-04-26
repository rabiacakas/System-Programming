/*******************************************************************************
*																			   *
*		 		 		RABİA ÇAKAS- SYSTEM PROGRAMMING HW3-				   *
*																			   *
*******************************************************************************/
#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>

#define MAX_PATH 4096
#define MAX_NAME 4096
#define FIFO_PERMS (S_IRUSR | S_IWUSR)


/*******************************************************************************
*	Bu fonksiyon kitaptan alinmistir, alinan path eger directory ise 		   *
* 1 degilse 0 return eder.													   *
*******************************************************************************/
int isDirectory(char *pathName);
/*******************************************************************************
*	Bu fonksiyon gelen directory name e gore acar okur eger actigi directory   *
* icinde bir directory ile karsilasirsa recursive olarak kendini cagirir, file *
* a rastladigi zaman ilk odev icin kullanilan file icinde arama fonksiyonunu   *
* cagirir.																	   *
*******************************************************************************/
int direcFind(char *dirName, char *string);
/*******************************************************************************
* Bu fonksiyon ilk odevde kullanilan file icinde arama fonksiyonudur		   *
*******************************************************************************/
int findInFile(FILE *inputP, FILE *inputLog, char* fileName, const char* word);

int main(int argc, char *argv[]){

	int dirOrOther;
	DIR *dirP=NULL;
	FILE *inputLog=NULL;
	int totalCount=0;
	char c;
	
	
	if(argc!= 3){

		fprintf(stderr, "The usage %s string can be that form : ./withPipeandFIFO string dirName.", argv[0]);
		return -1;

	}
	else if(strcmp(argv[0], "./withPipeandFIFO")!= 0){

		fprintf(stderr, "ARGUMENT ERROR: %s\n", argv[0]);
		return -1;
	}
	else if((dirP=opendir(argv[2]))==NULL){

		fprintf(stderr, "OPENING ERROR : %s -> %s\n",argv[2],strerror(errno));
		return -1;
	}
	else{

		dirOrOther=isDirectory(argv[2]);
		if(dirOrOther==1){
			inputLog=fopen("log.log", "a+");
			direcFind(argv[2], argv[1]);
			c=fgetc(inputLog);
			while(c!=EOF){
				if(c=='\n'){
					totalCount++;
				}
				c=fgetc(inputLog);
			}
			fprintf(inputLog, "%d %s were found in total\n", totalCount, argv[1]);
	}
	
	}
	if(inputLog){
		fclose(inputLog);
	}
	closedir(dirP);
	dirP=NULL;
	return 0;
}
int isDirectory(char *pathName){

	struct stat statbuf;
	if (stat(pathName, &statbuf) == -1){
		return 0;
	}
	else
		return S_ISDIR(statbuf.st_mode);
}
int direcFind(char *dirName, char *string){

	DIR * dirP= NULL;
	FILE * inputP=NULL;
	FILE *inputLog=NULL;
	pid_t chPid;
	int fd[2];
	struct dirent *readDir=NULL;
	char dirPath[MAX_PATH];
	char * fifoName=NULL;
	int flagFifo=0, retMkFifo=0;
	int *f_fd=0;
	int ncount=0, sumOfCount=0;

	if((dirP=opendir(dirName)) == NULL){
		
		fprintf(stderr, "Could not open directory: %s\n", dirName);
		return -1;
	}

	
	while((readDir=readdir(dirP)) != NULL){

        if (strcmp(readDir->d_name, ".")!=0 && strcmp(readDir->d_name, "..")!=0 ){
      
			strcpy(dirPath, dirName);
			strcat(dirPath, "/");
			strcat(dirPath, readDir->d_name);

			
			if(isDirectory(dirPath)==0){
				
				if(pipe(fd) == -1){
								
					fprintf(stderr, "Failed in pipe! %s\n", strerror(errno));
					return -1;	
				}
			}	
			if(isDirectory(dirPath)==1){
				fifoName =(char*)calloc(MAX_NAME, sizeof(char));

				if((retMkFifo=mkfifo(fifoName, FIFO_PERMS))==0)
				{
					flagFifo++;
				}
				else{
					flagFifo=0;
				}

			}		
			chPid=fork();
			if(chPid<0){

				fprintf(stderr, "Failed the fork! %s\n", strerror(errno));
				if(flagFifo!=0){

					unlink(fifoName);
					free(fifoName);
					fifoName= NULL;
				}
				else{
					
					close(fd[0]);
					close(fd[1]);
				}

				
				closedir(dirP);
				return -1;
			}
			else if(chPid==0){
				
				if(isDirectory(dirPath)==1){

					direcFind(dirPath, string);
					*f_fd=open(fifoName, FIFO_PERMS);
					
					if(write(*f_fd, &ncount, sizeof(ncount))<0){
						fprintf(stderr, "Write in fifo error : %s\n", strerror(errno));
					}
		
				}

				
				else if(isDirectory(dirPath)==0){
					close(fd[0]);
					ncount=findInFile(inputP, inputLog, dirPath, string);
					if(write(fd[1], &ncount, sizeof(ncount))<0){
						fprintf(stderr, "Write in pipe error : %s\n",strerror(errno));
					}
					close(fd[1]);
				}
				
				exit(0);
			}
	
	}
}
	
	while((wait(NULL))>0);
	close(fd[1]);
	while(read(fd[0], &ncount, sizeof(ncount))>0){
		sumOfCount+=ncount;
		
	}

	
	if(f_fd){
		free(f_fd);
	}
	close(fd[0]);
	close(fd[1]);
	if(fifoName){
		free(fifoName);
	}
	
	closedir(dirP);
	fifoName=NULL;
	f_fd=NULL;
	dirP=NULL;
	readDir=NULL;

	return (sumOfCount);
}

int findInFile(FILE *inputP, FILE *inputLog, char* fileName, const char* word){

	char c;
	int size = 0, newLineCounter = 1, maxLineLength = 0, max = 0,i,column = 1,
	row = 1, countOfword = 0, j, sizeofWord,k, sizeRev=0, h=0, x=0;
	char * totalArray=NULL;
	char *word1=NULL;
	char *revstr=NULL;

	if((inputP= fopen(fileName, "r"))== NULL){

		fprintf(stderr, "OPENING ERROR: %s > %s\n", fileName, strerror(errno));
		return -1;	
	}
	if((inputLog= fopen("log.log", "a"))== NULL){

		fprintf(stderr, "OPENING ERROR FROM log.log : %s\n", strerror(errno));
		return -1;
	}

	sizeRev=strlen(fileName);
	for(h=sizeRev-1; fileName[h]!='/'; --h){}

	revstr=(char*)calloc(sizeRev-h-1, sizeof(char));
	for(k=h+1; k<sizeRev; k++){

		revstr[x]=fileName[k];
		x++;
	}
	revstr[x]='\0';

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
				fprintf(inputLog, "%s: [%d, %d] %s first character found\n", revstr, row, column, word);
				countOfword++;
			}
			column++;
		}
	}
	
	totalArray[size]='\0';
	if(revstr){
		free(revstr);
	}
	if(totalArray){
		free(totalArray);}
	if(word1){
		free(word1);
	}
	
	totalArray=NULL;
	word1=NULL;
	/*fclose(inputLog);*/
	return countOfword;
}
