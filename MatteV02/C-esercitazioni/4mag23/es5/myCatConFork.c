#include <stdio.h>	// Includo la libreria per la funzione printf
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close, fork, execl
#include <fcntl.h>	// Includo la libreria per la funzione open e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait


int main(int argc, char** argv) {
    if (argc != 2) /* controllo sul numero di parametri: devono essere in numero uguale a 1*/
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
    close(fd);

    int pidChild;	// memorizzo il valore di ritorno della funzione fork
    
    if ((pidChild = fork()) < 0)	/* Il processo padre crea un figlio */
    {	/* In questo caso la fork e' fallita */
        printf("Errore durante la fork\n");
        exit(3);
    }
    
    if (pidChild == 0)
    {	/* processo figlio */
        close(0);
        open(argv[1], O_RDONLY);

        execl("mycat", "mycat", NULL);
    }
    /* processo padre */
    
    int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
    int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
    if ((pidChild = wait(&status)) < 0) {
        printf("Non e' stato creato nessun processo figlio\n");
        exit(4);
    }
    
    if ((status & 0xFF) != 0)
    {
        printf("Il processo figlio è stato terminato in modo anomalo\n");
        exit(4);
    } else {
        ritorno = (status >> 8) & 0xFF;
    }
    
    printf("Il pid del figlio e' %d e ha ritornato %d\n", pidChild, ritorno);

    exit(0);
}