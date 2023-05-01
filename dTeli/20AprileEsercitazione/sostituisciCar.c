#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <fcntl.h>
#include <string.h>


int main(int argc, char** argv){
    if (argc < 3) /* controllo sul numero di parametri: devono essere in numero uguale a 2*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(2);
    }
    //controllo se il file e' accedibile
    int fd = 0; /*variabile che conterra il file descriptor del file che stiamo per aprire */
    if((fd = open(argv[1], O_RDWR)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
        printf("Errore in apertura file %s dato che fd = %d\n", argv[1], fd);
        exit(1);
    }
    if (strlen(argv[2]) != 1)	/* controlliamo che il argv[2] sia un singolo carattere: N.B. Non viene chiesto che sia un carattere alfabetico o di una particolare categoria! */
    {
        printf("Errore: non singolo carattere dato che argv[2] = %s\n", argv[2]);
        exit(2);}
    char Cx=argv[2][0];

    char c;
    
    while (read(fd, &c, 1) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
    {
        if(c==Cx){
            lseek(fd,-1,SEEK_CUR);
            write(fd," ",1);
        }
    }

    exit(0);
}