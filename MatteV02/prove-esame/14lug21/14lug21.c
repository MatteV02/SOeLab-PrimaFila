#include <stdio.h>		// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>		// Includo la libreria per la funzione exit
#include <unistd.h>		// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe,
#include <fcntl.h>		// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <string.h>


//definisco il tipo pipe_t
typedef int pipe_t[2];

int main(int argc, char** argv) {
    if (argc < 3 + 1) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 3*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }

    int N = argc - 2;

    int nroLinee;		/*Dichiarazione varibile che conterra' argv[argc - 1]*/
    //controllo utlizzando la funzione atoi se il argv[argc - 1] e un numero strettamente positivo
    if((nroLinee = atoi(argv[argc - 1])) <= 0){
        printf("il parametro %s non un numero positivo\n",argv[argc - 1]);
        exit(2);
    }

    //creo il file "VERONI"
    int fcreato = 0; // variabile che conterra il file descriptor del file "VERONI" */
    if((fcreato = creat("VERONI", 0644)) < 0){		/* ERRORE se non si riesce a creare il file */
        printf("Errore in creazione del file %s dato che fcreato = %d\n", "VERONI", fcreato);
        exit(3);
    }

    char** tutteLinee = malloc(N * sizeof(*tutteLinee));
    if (tutteLinee == NULL) {
        printf("Errore nell'allocazione della memoria nel processo padre\n"); 
        exit(4);
    }
    for (int i = 0; i < N; i++)
    {
        tutteLinee[i] = malloc(250);
        if (tutteLinee[i] == NULL) {
            exit(4);
        }
    }
    

    //apro N pipes
    pipe_t* piped = (pipe_t*)malloc(sizeof(pipe_t) * (N));
    if (piped == NULL) {
        printf("Errore nell'allocazione della memoria\n");
        exit(5);
    }
    
    for(int i = 0; i < N; i++){
        if(pipe(piped[i])<0){
            printf("Errore nella creazione della pipe numero:%d\n",i);
            exit(5);
        }
    }

    for (int n = 0; n < N; n++)
    {
        int pidFiglio;	// memorizza il valore di ritorno della funzione fork
        
        if ((pidFiglio = fork()) < 0)	/* Il processo padre crea un figlio */
        {	/* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(6);
        }
        
        if (pidFiglio == 0)
        {	/* processo figlio */
            char c;	// carattere usato per leggere il contenuto del file
            char linea[250];
            int linea_size = 0;
            //chiudo tutte le pipe in lettura e scrittura tranne la n-esima pipe, aperta in lettura
            for(int k = 0; k < N; k++){
                if (k != n) {
                    close(piped[k][0]);
                    close(piped[(k + 1) % N][1]);
                }
            }

            //apro il file argv[n +  1]
            int fd = 0; // variabile che conterra' il file descriptor del file argv[n +  1]
            if((fd = open(argv[n +  1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire il file */
                printf("Errore in apertura file %s dato che fd = %d\n", argv[n +  1], fd);
                exit(255);
            }

            for (int nl = 0; nl < nroLinee; nl++)
            {      
                linea_size = 0;          
                while (read(fd, &c, sizeof(char)) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
                {
                    linea[linea_size++] = c;
                    if (c == '\n') {
                        for (int i = 0; i < n; i++)
                        {
                            read(piped[n][0], tutteLinee[i], 250);
                        }

                        memcpy(tutteLinee[n], linea, linea_size);

                        for (int i = 0; i < n + 1; i++)
                        {
                            write(piped[(n + 1) % N][1], tutteLinee[i], linea_size);
                        }
                        break;
                    }
                }
            }

            exit(linea_size);
        }
        /* processo padre */
        
    }
    
    //chiudo tutte le pipe in lettura e scrittura tranne la 0-esima pipe, aperta in lettura
    for(int k = 0; k < N; k++){
        close(piped[k][1]);
        if (k != 0) {
            close(piped[k][0]);
        }
    }

    for (int nl = 0; nl < nroLinee; nl++)
    {
        for (int n = 0; n < N; n++)
        {     
            char c;	// carattere usato per leggere il contenuto del file
            int linea_size = 0;       

            while (read(piped[0][0], &c, sizeof(char)) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
            {
                tutteLinee[n][linea_size++] = c;
                if (c == '\n') {
                    break;
                }
            }
            
            write(fcreato, tutteLinee[n], linea_size);
        }
    }

    for (int n = 0; n < N; n++)
    {
        int pidFiglio;
        int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
        int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
        if ((pidFiglio = wait(&status)) < 0) {
            printf("Non e' stato creato nessun processo figlio\n");
            exit(7);
        }
        
        if ((status & 0xFF) != 0)
        {
            printf("Il processo figlio con PID %d è stato terminato in modo anomalo\n", pidFiglio);
        } else {
            ritorno = (status >> 8) & 0xFF;
            printf("Il figlio di PID %d ha ritornato %d\n",pidFiglio,ritorno);
        }
    }    

    exit(0);
}