#include <stdio.h>	// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close, fork, exec, read, write
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <stdbool.h>
#include <sys/types.h>
#include <signal.h>
#include <fcntl.h>	// Includo la libreria per la funzione open e le relative macro

//definisco il tipo pipe_t
typedef int pipe_t[2];


int main(int argc, char** argv) {
    if (argc < 3 + 1) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 3*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }

    int N = argc - 2;

    bool *terminati = calloc(N, sizeof(*terminati));
    int *pidFigli = calloc(N, sizeof(*pidFigli));

    //apro N pipes
    pipe_t* pipedPF = (pipe_t*)malloc(sizeof(pipe_t) * (N));
    if (pipedPF == NULL) {
        printf("Errore nell'allocazione della memoria\n");
        exit(2);
    }
    
    for(int i = 0;i < N;i++){
        if(pipe(pipedPF[i])<0){
            printf("Errore nella creazione della pipe numero:%i\n",i);
            exit(2);
        }
    }

    //apro N pipes
    pipe_t* pipedFP = (pipe_t*)malloc(sizeof(pipe_t) * (N));
    if (pipedFP == NULL) {
        printf("Errore nell'allocazione della memoria\n");
        exit(3);
    }
    
    for(int i = 0;i < N;i++){
        if(pipe(pipedFP[i])<0){
            printf("Errore nella creazione della pipe numero:%i\n",i);
            exit(3);
        }
    }

    //controllo se il file e' accedibile
    int FA = 0; /*variabile che conterra il file descriptor del file che stiamo per aprire */
    if((FA = open(argv[argc - 1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
        printf("Errore in apertura file %s dato che fd = %d\n", argv[argc - 1], FA);
        exit(4);
    }

    for (int i = 0; i < N; i++) {
        int pidFiglio;	// memorizzo il valore di ritorno della funzione fork
        
        if ((pidFiglio = fork()) < 0)	/* Il processo padre crea un figlio */
        {	/* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(5);
        }
        
        if (pidFiglio == 0)
        {	/* processo figlio */
            close(FA);

            //chiudo tutte le pipe in lettura e scrittura tranne la i-esima pipe, aperta in scrittura
            for (int k = 0; k < N; k++) {
                close(pipedPF[k][1]);
                close(pipedFP[k][0]);
                if (k != i) {
                    close(pipedPF[k][0]);
                    close(pipedFP[k][1]);
                }
            }

            int ret_read;

            bool continua;

            //controllo se il file e' accedibile
            int fd = 0; /*variabile che conterra il file descriptor del file che stiamo per aprire */
            if((fd = open(argv[i + 1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
                printf("Errore in apertura file %s dato che fd = %d\n", argv[i + 1], fd);
                exit(255);
            }

            char c;
            
            while (1)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
            {
                ret_read = read(pipedPF[i][0], &continua, sizeof(continua));

                if (ret_read == 0) {
                    printf("Errore, il figlio %d non ha scrittori sulla pipe pipedPF\n", i); 
                    exit(254);
                }

                ret_read = read(fd, &c, sizeof(c));

                if (ret_read <= 0) {
                    break;
                }

                write(pipedFP[i][1], &c, sizeof(c));
            }

            exit(0);
        }
        /* processo padre */
        pidFigli[i] = pidFiglio;
    }

    //chiudo tutte le pipe in lettura e scrittura tranne la i-esima pipe, aperta in scrittura
    for(int k = 0; k < N; k++){
        close(pipedPF[k][0]);
        close(pipedFP[k][1]);
    }

    char c;
    char carattere_file_padre;
    int ret_read;

    while (1) {
        for (int i = 0; i < N; i++) {
            if (!terminati[i]) {
                bool continua = true;

                write(pipedPF[i][1], &continua, sizeof(continua));
            }
        }

        ret_read = read(FA, &carattere_file_padre, sizeof(carattere_file_padre));

        if (ret_read < 0) {
            printf("Errore, in lettura dal file %s\n", argv[argc - 1]); 
            exit(6);
        }

        if (ret_read == 0) {
            break;
        }

        for (int  i = 0; i < N; i++)
        {
            if (!terminati[i]) {
                if (read(pipedFP[i][0], &c, sizeof(c)) == 0) {
                    printf("Errore, la pipe %d è chiusa sul lato del figlio\n", i); 
                    exit(7);
                }

                if (c != carattere_file_padre) {
                    terminati[i] = true;
                }
            }
        }
    }

    for (int i = 0; i < N; i++)
    {
        if (terminati[i] == true) {
            kill(pidFigli[i], SIGKILL);
        }
    }
    
    for (int i = 0; i < N; i++) {
        int pidFiglio;
        int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
        int ritorno;
        if ((pidFiglio = wait(&status)) < 0) {
            printf("Non e' stato creato nessun processo figlio\n");
            exit(8);
        }
        
        if ((status & 0xFF) != 0)
        {
            printf("Il processo figlio %d è stato terminato in modo anomalo, segnale %d\n", pidFiglio, status & 0xFF);
        } else {
            ritorno = (ritorno >> 8) & 0xFF;

            int k = 0;
            while (pidFigli[k] != pidFiglio)
            {
                k++;
            }
            
            printf("Il processo figlio %d è stato terminato in modo normale. Il figlio ha ritornato %d. Quindi il file %s e' uguale al file %s\n", pidFiglio, ritorno, argv[k + 1], argv[argc - 1]);
        }
    }
    

    exit(0);
}