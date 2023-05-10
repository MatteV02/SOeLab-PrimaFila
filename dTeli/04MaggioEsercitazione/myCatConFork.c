#include <stdio.h>	// Includo la libreria per la funzione printf
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close e fork
#include <fcntl.h>	// Includo la libreria per la funzione open e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait


int main(int argc, char** argv) {
    /* processo padre */
    if (argc != 2) /* controllo sul numero di parametri: devono essere in numero uguale a 1*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }
    char* arg[2];
    arg[0]="mycat";
    arg[1]=0;
    int pidChild;	// memorizzo il valore di ritorno della funzione fork
    
    if ((pidChild = fork()) < 0)	/* Il processo padre crea un figlio */
    {	/* In questo caso la fork e' fallita */
        printf("Errore durante la fork\n");
        exit(2);
    }
    
    if (pidChild == 0)
    {	/* processo figlio */
        close(0);
        //controllo se il file e' accedibile
        
        if((open(argv[1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
            printf("Errore in apertura file %s \n", argv[1]);
            exit(2);
        }
        execv("mycat",arg);
        exit(0);
    }
    int pidEnd;
    int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
    int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
    if ((pidEnd = wait(&status)) < 0) {
        printf("Non e' stato creato nessun processo figlio\n");
        exit(2);
    }
    
    if ((status & 0xFF) != 0)
    {
        printf("Il processo figlio è stato terminato in modo anomalo\n");
        exit(1);
    } else {
        ritorno = (status >> 8) & 0xFF;
    }
    
    printf("pidFiglio=%d ritornato=%d\n",pidEnd,ritorno);
    exit(0);
}