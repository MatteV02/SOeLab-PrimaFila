#include <stdio.h>	// Includo la libreria per la funzione printf
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close, fork e exec
#include <fcntl.h>	// Includo la libreria per la funzione open e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <string.h>
//definisco il tipo pipe_t
typedef int pipe_t[2];

int main(int argc, char** argv) {
    if (argc < 4) /* controllo sul numero di parametri: devono essere in numero maggiore a 3*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }
    //controllo se il file e' accedibile

    pipe_t* piped=malloc(sizeof(pipe_t)*argc-2);
    for(int i=0;i<argc-2;i++){
    if (strlen(argv[i+2]) != 1) {	// #include <string.h>
        printf("Errore, la stringa %s non e' un singolo carattere\n", argv[i]);
        exit(3);
    }
    //apro argc-2 pipes
    char C = argv[i+2][0];
    int pidChild;	// memorizzo il valore di ritorno della funzione fork
    if(pipe(piped[i])<0){
            printf("Errore nella creazione della pipe numero:%i",i);
    }
    if ((pidChild = fork()) < 0)	/* Il processo padre crea un figlio */
    {	/* In questo caso la fork e' fallita */
        printf("Errore durante la fork\n");
        exit(4);
    }
    
    if (pidChild == 0)
    {	/* processo figlio */
        int fd = 0; /*variabile che conterra il file descriptor del file che stiamo per aprire */
        if((fd = open(argv[1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
            printf("Errore in apertura file %s dato che fd = %d\n", argv[1], fd);
            exit(2);
        }
        char N;
        //chiudo tutte le pipe in ${1|lettura|scrittura|}
        for(int k=0;k<=i;k++){
            close(piped[k][0]);
        }
        //chiudo tutte le pipe in ${1|lettura|scrittura|}
        for(int k=0;k<i;k++){
            close(piped[k][1]);
        }
        long int contatore = 0;
        while (read(fd, &N, 1) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
        {
            if(N==C){
                contatore++;
            }
        }
        write(piped[i][1],&contatore,sizeof(long int));
        exit(C);
    }

    }
    /* processo padre */
    //chiudo tutte le pipe in ${1|lettura|scrittura|}
    for(int k=0;k<argc-2;k++){
        close(piped[k][1]);
    }
    for(int i=0;i<argc-2;i++){
    int pidchild;
    int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
    int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
    if ((pidchild = wait(&status)) < 0) {
        printf("Non e' stato creato nessun processo figlio\n");
        exit(5);
    }
    
    if ((status & 0xFF) != 0)
    {
        printf("Il processo figlio è stato terminato in modo anomalo\n");
        exit(5);
    } else {
        ritorno = (status >> 8) & 0xFF;
    }

    printf("il figlio pid=%i ed ha ritornato=%c\n",pidchild,ritorno);
    long int maxread=0;
    if(read(piped[i][0],&maxread,sizeof(long int))>0){
        printf("numero di carateri trovato=%li \n",maxread);
    }

    }
    
    
    

    exit(0);
}