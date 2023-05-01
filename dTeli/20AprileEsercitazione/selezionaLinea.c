#include <stdio.h>	// Includo la libreria per la funzione printf
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close e fork
#include <fcntl.h>	// Includo la libreria per la funzione open e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait


int main(int argc, char** argv) {
    if (argc != 3) /* controllo sul numero di parametri: devono essere in numero uguale a 2*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }
    //controllo se il file e' accedibile
    int fd = 0; /*variabile che conterra il file descriptor del file che stiamo per aprire */
    if((fd = open(argv[1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
        printf("Errore in apertura file %s dato che fd = %d\n", argv[1], fd);
        exit(2);
    }
    int n;		/*Inizializzo varibile che conterra' argv[2]*/
    //controllo utlizzando la funzione atoi se il argv[2] e un numero strettamente positivo
    if((n=atoi(argv[2])) <= 0){
        printf("il parametro %s non un numero positivo\n",argv[2]);
        exit(3);
    }

    char c;
    int contaCR=1;
    while (read(fd, &c, 1) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
    {
        if(contaCR==n){
            printf("%c",c);
        }
        if(c=='\n'){
            contaCR++;
        }
        if(contaCR>n){
            break;
        }
        
    }
    exit(0);
}