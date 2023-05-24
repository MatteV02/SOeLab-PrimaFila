#include <stdio.h>	// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe,
#include <fcntl.h>	// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <ctype.h>  // Includeo la libreria ctype per la funzione isdigit

// il padre lascia aperti i lati di lettura della prima pipe perché altrimenti le write su processi terminati terminerebbero i processi figli

//definisco il tipo pipe_t
typedef int pipe_t[2];

int main(int argc, char** argv) {
    int N = argc - 1;   // memorizzo il numero di parametri passati
    char continua = 'c';

    if (argc > 2 + 1) /* controllo sul numero di parametri: devono essere in numero minore o uguale a 2*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }

    //apro N pipes
    pipe_t* piped = (pipe_t*)malloc(sizeof(pipe_t) * (N));
    if (piped == NULL) {
        printf("Errore nell'allocazione della memoria\n");
        exit(2);
    }
    
    for(int i = 0;i < N;i++){
        if(pipe(piped[i])<0){
            printf("Errore nella creazione della pipe numero:%i\n",i);
            exit(2);
        }
    }

    for (int i = 0; i < N; i++)
    {
        int pidFiglio;	// memorizzo il valore di ritorno della funzione fork
        
        if ((pidFiglio = fork()) < 0)	/* Il processo padre crea un figlio */
        {	/* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(3);
        }
        
        if (pidFiglio == 0)
        {	/* processo figlio */
            //chiudo tutte le pipe in lettura e scrittura tranne la i-esima pipe, aperta in scrittura
            for(int k = 0; k < N; k++){
                if (k != i) {
                    close(piped[k][0]);
                }
                if (k != (i + 1) % N) {
                    close(piped[k][1]);
                }
            }

            int pipe_scrivi = piped[(i + 1) % N][1];
            int pipe_leggi = piped[i][0];

            //controllo se il file e' accedibile
            int fd = 0; /*variabile che conterra il file descriptor del file che stiamo per aprire */
            if((fd = open(argv[i + 1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
                printf("Errore in apertura file %s dato che fd = %d\n", argv[i + 1], fd);
                exit(255);
            }

            char c;
            char linea[256];    // suppongo ogni linea sia al massimo lunga 256 caratteri
            int numero_cifre = 0;
            int linea_size = 0;
            
            while (read(fd, &c, 1) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
            {
                linea[linea_size++] = c;
                if (isdigit(c)) {
                    numero_cifre++;
                }
                if (c == '\n') {
                    linea[linea_size - 1] = 0;
                    read(pipe_leggi, &continua, sizeof(continua));
                    printf("Il figlio con numero d'ordine %d, PID %d, ha trovato nella linea \"%s\" %d caratteri numerici.\n", i, getpid(), linea, numero_cifre);
                    write(pipe_scrivi, &continua, sizeof(continua));
                    linea_size = 0;
                    numero_cifre = 0;
                }
            }

            exit(numero_cifre);
        }
        /* processo padre */
        
    }
    
    //chiudo tutte le pipe in lettura e scrittura tranne la 0-esima pipe, aperta in lettura
    for(int k = 0; k < N; k++){
        if (k != 0) {
            close(piped[k][1]);
            close(piped[k][0]);
        }
    }

    write(piped[0][1], &continua, sizeof(continua));

    for (int i = 0; i < N; i++)
    {
        int pidFiglio;
        int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
        int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
        if ((pidFiglio = wait(&status)) < 0) {
            printf("Non e' stato creato nessun processo figlio\n");
            exit(4);
        }
        
        if ((status & 0xFF) != 0)
        {
            printf("Il processo figlio è stato terminato in modo anomalo\n");
        } else {
            ritorno = (status >> 8) & 0xFF;
            printf("il figlio pid=%i ed ha ritornato=%i\n",pidFiglio,ritorno);
        }
    }

    exit(0);
}