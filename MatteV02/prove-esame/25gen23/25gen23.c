#include <stdio.h>		// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>		// Includo la libreria per la funzione exit
#include <unistd.h>		// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe,
#include <fcntl.h>		// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <stdbool.h>


//definisco il tipo pipe_t
typedef int pipe_t[2];

int main(int argc, char** argv) {
    if (argc < 3 + 1) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 3*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }

    //apro il file argv[argc - 1]
    int fdw = 0; // variabile che conterra' il file descriptor del file argv[argc - 1]
    if((fdw = open(argv[argc - 1], O_WRONLY | O_TRUNC)) < 0){		/* ERRORE se non si riesce ad aprire il file */
        printf("Errore in apertura file %s dato che fdw = %d\n", argv[argc - 1], fdw);
        exit(2);
    }

    char chs[2];
    int N = argc - 2;

    //apro N pipes
    pipe_t* pipedFP = (pipe_t*)malloc(sizeof(pipe_t) * (N));
    if (pipedFP == NULL) {
        printf("Errore nell'allocazione della memoria\n");
        exit(3);
    }
    
    for(int i = 0; i < N; i++){
        if(pipe(pipedFP[i])<0){
            printf("Errore nella creazione della pipe numero:%d\n",i);
            exit(3);
        }
    }

    for (int n = 0; n < N; n++)
    {
        int pidFiglio;	// memorizza il valore di ritorno della funzione fork
        
        if ((pidFiglio = fork()) < 0)	/* Il processo padre crea un figlio */
        {	/* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(4);
        }
        
        if (pidFiglio == 0)
        {	/* processo figlio */
            //chiudo tutte le pipe in lettura e scrittura tranne la n-esima pipe, aperta in scrittura
            for(int k = 0; k < N; k++){
                close(pipedFP[k][0]);
                if (k != n) {
                    close(pipedFP[k][1]);
                }
            }

            //apro il file argv[n + 1]
            int fd = 0; // variabile che conterra' il file descriptor del file argv[n + 1]
            if((fd = open(argv[n + 1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire il file */
                printf("Errore in apertura file %s dato che fd = %d\n", argv[n + 1], fd);
                exit(255);
            }

            int ret_read;
            int nCaratteriInviati = 0;

            while ((ret_read = read(fd, chs, 2)) > 0)
            {
                nCaratteriInviati += ret_read;
                write(pipedFP[n][1], chs, ret_read);
            }

            exit(nCaratteriInviati);
        }
        /* processo padre */
        
    }

    //chiudo tutte le pipe in lettura e scrittura tranne la i-esima pipe, aperta in lettura
    for(int k = 0; k < N; k++){
        close(pipedFP[k][1]);
    }

    bool continua = true;

    while (continua) {
        continua = false;
        int ret_read;
        for (int n = 0; n < N; n++)
        {
            if ((ret_read = read(pipedFP[n][0], chs, 2)) > 0) {
                continua = true;
                write(fdw, chs, ret_read);
            }
        }
        
    }
    
    for (int n = 0; n < N; n++)
    {
        int pidFiglio;
        int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
        int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
        if ((pidFiglio = wait(&status)) < 0) {
            printf("Non e' stato creato nessun processo figlio\n");
            exit(5);
        }
        
        if ((status & 0xFF) != 0)
        {
            printf("Il processo figlio è stato terminato in modo anomalo\n");
        } else {
            ritorno = (status >> 8) & 0xFF;
            printf("Il figlio di PID %d ha ritornato %d\n",pidFiglio,ritorno);
        }
    }
    

    exit(0);
}