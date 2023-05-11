#include <stdio.h>	// Includo la libreria per la funzione printf
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close, fork e exec
#include <fcntl.h>	// Includo la libreria per la funzione open e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <string.h>

//definisco il tipo pipe_t
typedef int pipe_t[2];

int main(int argc, char** argv) {
    if (argc < 4) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 3*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }

    int N = argc - 2;

    char* Cf = malloc(N);

    for (int i = 0; i < N; i++) {
        if (strlen(argv[i + 2]) != 1) {	// #include <string.h>
            printf("Errore, la stringa %s non e' un singolo carattere\n", argv[i]);
            exit(3);
        }
        char Cx = argv[i + 2][0];
        Cf[i] = Cx;
    }


    //apro N pipes
    pipe_t* piped=malloc(sizeof(pipe_t)*N);
    for(int i=0;i<N;i++){
        if(pipe(piped[i])<0){
            printf("Errore nella creazione della pipe numero:%i",i);
        }
    }

    for (int i = 0; i < N; i++)
    {
        int Pi;	// memorizzo il valore di ritorno della funzione fork
        
        if ((Pi = fork()) < 0)	/* Il processo padre crea un figlio */
        {	/* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(4);
        }
        
        if (Pi == 0)
        {	/* processo figlio */
            //chiudo tutte le pipe che non uso
            //controllo se il file e' accedibile
            int fd = 0; /*variabile che conterra il file descriptor del file che stiamo per aprire */
            if((fd = open(argv[1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
                printf("Errore in apertura file %s dato che fd = %d\n", argv[1], fd);
                exit(2);
            }

            for(int k=0; k < N; k++) {
                close(piped[k][0]);
                if (k != i) {
                    close(piped[k][1]);
                }
            }

            char buffer;
            long int count = 0;
            
            while (read(fd, &buffer, 1) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
            {
                if (buffer == Cf[i]) {
                    count++;
                }
            }

            write(piped[i][1], &count, sizeof(count));

            exit(buffer);
        }
        /* processo padre */
        
    }

    //chiudo tutte le pipe che non uso
    for(int k=0; k < N; k++) {
        close(piped[k][1]);
    }

    for (int i = 0; i < N; i++)
    {
        long int count;
        if (read(piped[i][0], &count, sizeof(count)) > 0) {
            printf("%ld corrispondenze per la lettera %c nel file %s\n", count, Cf[i], argv[0]);
        }
    }
    
    for (int i = 0; i < N; i++)
    {
        int pidChild;
        int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
        int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
        if ((pidChild = wait(&status)) < 0) {
            printf("Non e' stato creato nessun processo figlio\n");
            exit(4);
        }
        
        if ((status & 0xFF) != 0)
        {
            printf("Il processo figlio è stato terminato in modo anomalo\n");
        } else {
            ritorno = (status >> 8) & 0xFF;
        }
        printf("il figlio pid=%i ed ha ritornato=%i\n",pidChild,ritorno);
    }
    

    exit(0);
}