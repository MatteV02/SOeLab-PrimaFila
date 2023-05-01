#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <fcntl.h>


int main(int argc, char** argv){
   if (argc < 3) /* controllo sul numero di parametri: devono essere in numero uguale a 2*/
   {	printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
   }
  int n;		/*Inizializzo varibile che conterra argv[2]*/
  //controllo utlizzando la funzione atoi se il argv[2] e un numero strettamente positivo
  if((n=atoi(argv[2]))<=0 && n>=0){
    printf("il parametro %s non un numero positivo",argv[2]);
    exit(2);
  }
  //controllo se il file e accedibile
  int fd=0; /*variabile che conterra il file descriptor del file che stiamo per aprire */
  if((fd = open(argv[1], O_RDWR)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
    printf("Errore in apertura file %s dato che fd = %d\n", argv[1], fd);
    exit(3);
  }
  char c=0;
  long int lungezzafile=lseek(fd,0L,SEEK_END);
  char* caratteri=(char*) malloc(lungezzafile/n);
  lseek(fd,0l,SEEK_SET);
  for(int i=0;i<lungezzafile/n;i++){
      lseek(fd,n-1,SEEK_CUR);
      read(fd,&c,1);
      caratteri[i]=c;
  }
  int contaOut=1;
  for(int i=0;i<lungezzafile/n;i++){
    printf("il carattere %d-esimo del file %s e %c\n",contaOut*n,argv[1],caratteri[i]);   
    contaOut++;
  }
  exit(0);
}