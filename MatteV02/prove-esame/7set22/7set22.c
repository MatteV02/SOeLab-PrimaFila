#include <stdio.h>		// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>		// Includo la libreria per la funzione exit
#include <unistd.h>		// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe,
#include <fcntl.h>		// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait


//definisco il tipo pipe_t
typedef int pipe_t[2];

int main(int argc, char** argv) {
    int N = argc - 1;   // numero di file passati come parametro
    int fcreato = 0;    // variabile che conterra il file descriptor del file "VERONI" */
    char *cur;          // vettore usato per la comunicazione su pipe
    pipe_t* piped;      // vettore di pipe usate per la comunicazione tra i processi
    int pidFiglio;	    // memorizza il valore di ritorno della funzione fork e wait
    int status;	        // La variabile usata per memorizzare quanto ritornato dalla primitiva wait
    int ritorno;	    // La variabile usata per memorizzare il valore di ritorno del processo figlio


    if (argc < 2 + 1) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 2*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }

    //creo il file "VERONI"
    if((fcreato = creat("VERONI", 0644)) < 0){		/* ERRORE se non si riesce a creare il file */
        printf("Errore in creazione del file %s dato che fcreato = %d\n", "VERONI", fcreato);
        exit(2);
    }

    // alloco il vettore cur controllando che la memoria venga effettivamente allocata
    cur = malloc(sizeof(*cur) * N);
    if (cur == NULL) {
        printf("Errore nell'allocazione della memoria\n"); 
        exit(3);
    }

    //apro N pipes
    piped = (pipe_t*)malloc(sizeof(pipe_t) * (N));
    if (piped == NULL) {
        printf("Errore nell'allocazione della memoria\n");
        exit(4);
    }
    
    for(int i = 0; i < N; i++){
        if(pipe(piped[i])<0){
            printf("Errore nella creazione della pipe numero:%d\n",i);
            exit(4);
        }
    }

    for (int n = 0; n < N; n++) // genero N processi figli
    {        
        if ((pidFiglio = fork()) < 0)	/* Il processo padre crea un figlio */
        {	/* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(5);
        }
        
        if (pidFiglio == 0)
        {	/* processo figlio */
            int fd = 0;             // variabile che conterra' il file descriptor del file argv[n + 1]
            char car;	            // carattere usato per leggere il contenuto del file
            char ultimoDispari = 0; // variabile che conterrà l'ultimo carattere dispari trovato; non è un'operazione necessaria perché car conterrà alla fine delle letture l'ultimo carattere dispari, ma compio questa operazione per sicurezza
            int count = 1;          // contatore per individuare quale carattere è dispari


            //chiudo tutte le pipe in lettura e scrittura tranne la n-esima pipe aperta in lettura e la (n + 1) % N - esima aperta in scrittura
            for(int k = 0; k < N; k++){
                if (k != n) {
                    close(piped[k][0]);
                    close(piped[(k + 1) % N][1]);
                }
            }

            if (n == 0) {
                close(piped[0][0]); // chiudo la pipe piped[0][0] che verrà letta dal padre
            }

            //apro il file argv[n + 1]
            if((fd = open(argv[n + 1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire il file */
                printf("Errore in apertura file %s dato che fd = %d\n", argv[n + 1], fd);
                exit(255);
            }

            while (read(fd, &car, sizeof(char)) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
            {
                if ((count % 2) == 1) {
                    // il carattere letto è dispari
                    if (n != 0) {
                        // leggo l'array cur dal figlio precedente (se il processo attuale è il figlio 0 salto questa operazione)
                        read(piped[n][0], cur, N);
                    }

                    cur[n] = car;   // metto in coda all'array cur il carattere letto

                    write(piped[(n + 1) % N][1], cur, N);   // mando il carattere al figlio successivo
                    ultimoDispari = car;    // memorizzo il carattere letto come ultimo carattere dispari letto
                }
                count++;
            }
            
            exit(ultimoDispari);    // ritorno al padre l'ultimo carattere dispari letto
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

    while(read(piped[0][0], cur, N) > 0) {
        // leggo tutti gli array preparati dai figli e li scrivo sul file creato
        write(fcreato, cur, N);
    }

    for (int i = 0; i < N; i++) // attendo la terminazione dei processi figli e stampo le informazioni di ritorno dei processi figli
    {
        if ((pidFiglio = wait(&status)) < 0) {
            printf("Non e' stato creato nessun processo figlio\n");
            exit(6);
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