#include <stdio.h>		// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>		// Includo la libreria per la funzione exit
#include <unistd.h>		// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe,
#include <fcntl.h>		// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <string.h>


//definisco il tipo pipe_t
typedef int pipe_t[2];

int main(int argc, char** argv) {
    if (argc < 2 + 1) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 2*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }

    int C;		/*Dichiarazione varibile che conterra' argv[argc - 1]*/
    //controllo utlizzando la funzione atoi se il argv[argc - 1] e un numero strettamente positivo
    if((C = atoi(argv[argc - 1])) <= 0 || C % 2 == 0){
        printf("il parametro %s non un numero positivo o non e' dispari\n",argv[argc - 1]);
        exit(2);
    }

    int N = argc - 2;

    char *b = malloc(C);
    if (b == NULL) {
        printf("Errore nell'allocazione della memoria\n"); 
        exit(255);
    }

    //apro N pipes
    pipe_t* piped = (pipe_t*)malloc(sizeof(pipe_t) * (N));
    if (piped == NULL) {
        printf("Errore nell'allocazione della memoria\n");
        exit(3);
    }
    
    for(int i = 0; i < N; i++){
        if(pipe(piped[i])<0){
            printf("Errore nella creazione della pipe numero:%d\n",i);
            exit(3);
        }
    }

    for (int i = 0; i < 2 * N; i++)
    {
        int pidFiglio;	// memorizza il valore di ritorno della funzione fork
        
        if ((pidFiglio = fork()) < 0)	/* Il processo padre crea un figlio */
        {	/* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(4);
        }
        
        if (pidFiglio == 0)
        {	/* processo figlio */

            //chiudo tutte le pipe in lettura e scrittura tranne la i-esima pipe, aperta in scrittura
            for(int k = 0; k < N; k++){
                if (i >= N) {
                    close(piped[k][1]);
                } else {
                    close(piped[k][0]);
                }
                if (k != i % N) {
                    if (i >= N) {
                        close(piped[k][0]);
                    } else {
                        close(piped[k][1]);
                    }
                }
            }

            int fcreato = 0; // variabile che conterra il file descriptor del file creato */
            if (i >= N) {
                //creo il file argv[i - N]
                char *stringa = malloc(strlen(argv[i - N + 1]) + 11);
                sprintf(stringa, "%s.mescolato", argv[i - N + 1]);
                
                if((fcreato = creat(stringa, 0644)) < 0){		/* ERRORE se non si riesce a creare il file */
                    printf("Errore in creazione del file %s dato che fd = %d\n", stringa, fcreato);
                    exit(255);
                }
            }
            
            //apro il file argv[i % N + 1]
            int fd = 0; // variabile che conterra' il file descriptor del file argv[i % N + 1]
            if ((fd = open(argv[i % N + 1], O_RDONLY)) < 0) {		/* ERRORE se non si riesce ad aprire il file */
                printf("Errore in apertura file %s dato che fd = %d\n", argv[i % N + 1], fd);
                exit(255);
            }

            int lunghezzaInBlocchi = lseek(fd, 0, SEEK_END) / C;
            int nro;

            if (i >= N) {
                lseek(fd, (lunghezzaInBlocchi / 2) * C, SEEK_SET);
            } else {
                lseek(fd, 0, SEEK_SET);
            }

            nro = lunghezzaInBlocchi / 2;

            for (int i = 0; i < nro; i++) {
                read(fd, b, C);
                if (i >= N) {
                    printf("DEBUG:nipote %d. Scrivo su file1\n", i);
                    write(fcreato, b, C);
                    if (read(piped[i][0], b, C) == 0) {
                        break;
                    }
                    printf("DEBUG:figlio %d. Scrivo su file2\n", i);
                    write(fcreato, b, C);
                } else {
                    printf("DEBUG:figlio %d. Scrivo su pipe\n", i);
                    write(piped[i][1], b, C);
                }
            }

            exit(nro);
        }
        /* processo padre */
        
    }

    //chiudo tutte le pipe in lettura e scrittura
    for(int k = 0; k < N; k++){
        close(piped[k][1]);
        close(piped[k][0]);
    }

    for (int i = 0; i < 2 * N; i++)
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