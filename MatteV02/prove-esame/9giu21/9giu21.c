#include <stdio.h>		// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>		// Includo la libreria per la funzione exit
#include <unistd.h>		// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe,
#include <fcntl.h>		// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <errno.h>


//definisco il tipo pipe_t
typedef int pipe_t[2];

int main(int argc, char** argv) {
    int fcreato = 0;        // variabile che conterra il file descriptor del file "/tmp/MatteoVeroni" */
    pipe_t pipeWC;          // dichiaro pipe per ottenere quanto prodotto dal comando wc
    char c;	                // carattere usato per leggere carattere per carattere il contenuto dei file
    char outputWC[11];      // array usato per contenere il risultato del comando wc. Suppongo la sua lunghezza sia 11 in quanto un numero intero può essere rappresentato al massimo con 10 cifre decimali.
    int outputWC_size = 0;  // variabile intera che memorizza la dimensione di outputWC
    int lunghezzaLinee;     // variabile che memorizza la lunghezza in linee dei file passati per parametro
    int pidFiglio;	        // memorizza il valore di ritorno della funzione fork e wait
    char linea[201];        // array usato per memorizzare le linee dei file (suppongo una linea conti al massimo 200 caratteri)
    int linea_size = 0;     // variabile usata per determinare la dimensione di linea
    int N = argc - 1;       // numero di file passati per parametro
    pipe_t* pipedFP;        // array di pipe per lo scambio di dati tra figli e padre
    int status;	            // La variabile usata per memorizzare quanto ritornato dalla primitiva wait
    int ritorno;	        // La variabile usata per memorizzare il valore di ritorno del processo figlio

    
    if (argc < 2 + 1) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 2*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }

    //creo il file "/tmp/MatteoVeroni"
    if((fcreato = creat("/tmp/MatteoVeroni", 0644)) < 0){		/* ERRORE se non si riesce a creare il file */
        printf("Errore in creazione del file %s dato che fd = %d\n", "/tmp/MatteoVeroni", fcreato);
        exit(2);
    }
    
    if (pipe(pipeWC) < 0 )
    {	printf("Errore creazione pipe\n");
        exit(3);
    }
    
    if ((pidFiglio = fork()) < 0)	/* Il processo padre crea un figlio */
    {	/* In questo caso la fork e' fallita */
        printf("Errore durante la fork\n");
        exit(4);
    }
    
    if (pidFiglio == 0)
    {	/* processo figlio */
        int fd = 0; // variabile che conterra' il file descriptor del file argv[1]
        
        close(pipeWC[0]);

        // chiudo lo stdout
        close(1);
        // duplico la pipe lato  scrittura
        dup(pipeWC[1]);
        // chiudo il lato della pipe duplicato
        close(pipeWC[1]);
        
        close(0);
        //apro il file argv[1]
        if((fd = open(argv[1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire il file */
            printf("Errore in apertura file %s dato che fd = %d\n", argv[1], fd);
            exit(255);
        }

        execlp("wc", "wc", "-l", (char*)0); 
        /* si esegue l'istruzione seguente SOLO in caso di fallimento */
        
        perror("errore esecuzione comando"); 
        exit(errno);
    }
    /* processo padre */
    
    close(pipeWC[1]);
    
    while (read(pipeWC[0], &c, sizeof(char)) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
    {
        if (c == '\n') {
            outputWC[outputWC_size++] = 0;
            break;
        }
        outputWC[outputWC_size++] = c;
    }

    if ((lunghezzaLinee = atoi(outputWC)) <= 0) {
        printf("Errore, nella conversione dell'output di wc sul primo file passato\n"); 
        exit(5);
    }

    close(pipeWC[0]);

    if ((pidFiglio = wait((int*) 0)) < 0) {
        printf("Non e' stato creato nessun processo figlio\n");
        exit(6);
    }

    //apro N pipes
    pipedFP = (pipe_t*)malloc(sizeof(pipe_t) * (N));
    if (pipedFP == NULL) {
        printf("Errore nell'allocazione della memoria\n");
        exit(7);
    }
    
    for(int i = 0; i < N; i++) {
        if(pipe(pipedFP[i])<0){
            printf("Errore nella creazione della pipe numero:%d\n",i);
            exit(7);
        }
    }

    for (int n = 0; n < N; n++)
    {        
        int fd = 0; // variabile che conterra' il file descriptor del file argv[n + 1]
        if ((pidFiglio = fork()) < 0)	/* Il processo padre crea un figlio */
        {	/* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(8);
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
            if((fd = open(argv[n + 1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire il file */
                printf("Errore in apertura file %s dato che fd = %d\n", argv[n + 1], fd);
                exit(255);
            }

            for (int j = 0; j < lunghezzaLinee; j++)
            {
                linea_size = 0;
                while (read(fd, &c, sizeof(char)) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
                {
                    linea[linea_size++] = c;
                    if (c == '\n') {
                        break;
                    }
                }

                write(pipedFP[n][1], &linea_size, sizeof(linea_size));

                write(pipedFP[n][1], &linea, linea_size);
            }

            exit(linea_size);
        }
        /* processo padre */
        
    }
    
    //chiudo tutte le pipe in scrittura
    for(int k = 0; k < N; k++){
        close(pipedFP[k][1]);
    }

    for (int j = 0; j < lunghezzaLinee; j++)
    {
        for (int n = 0; n < N; n++)
        {
            read(pipedFP[n][0], &linea_size, sizeof(linea_size));

            read(pipedFP[n][0], &linea, linea_size);

            write(fcreato, &linea, linea_size);
        }
        
    }
    
    for (int n = 0; n < N; n++)
    {
        if ((pidFiglio = wait(&status)) < 0) {
            printf("Non e' stato creato nessun processo figlio\n");
            exit(9);
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