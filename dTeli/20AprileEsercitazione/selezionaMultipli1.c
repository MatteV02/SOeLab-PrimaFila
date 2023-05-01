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
  int conta=1;
  int contamalloc=1;
  char* caratteri=0;
  while(read(fd,&c,1)>0){
      if(conta%n==0){
        caratteri=realloc(caratteri,1*contamalloc);
        caratteri[contamalloc]=c;
        contamalloc++;
      }
      conta++;
  }

  for(int i=1;i<contamalloc;i++){
    printf("il carattere %d-esimo del file %s e %c\n",i*n,argv[1],caratteri[i]);   
  }
  exit(0);
}