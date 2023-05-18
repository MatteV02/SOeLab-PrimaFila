#include <stdio.h>	// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close, fork, exec, read, write
#include <fcntl.h>	// Includo la libreria per la funzione open e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <errno.h>

//definisco il tipo pipe_t
typedef int pipe_t[2];

int main(int argc, char** argv) {
    if (argc < 2 + 1) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 2*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }

    int N = argc - 1;

    //apro N pipes
    pipe_t* pipedPF = malloc(sizeof(pipe_t) * N);
    for(int i = 0;i < N; i++){
        if(pipe(pipedPF[i])<0){
            printf("Errore nella creazione della pipe numero:%i",i);
            exit(2);
        }
    }

    for (int i = 0; i < N; i++) {
        int pidChild;	// memorizzo il valore di ritorno della funzione fork
        
        if ((pidChild = fork()) < 0)	/* Il processo padre crea un figlio */
        {	/* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(3);
        }
        
        if (pidChild == 0)
        {	/* processo figlio */

            // chiudo pipe padre

            //chiudo tutte le pipe in lettura e scrittura tranne la i-esima pipe, aperta in scrittura
            for(int k = 0; k < N; k++){
                close(pipedPF[k][0]);
                if (k != i) {
                    close(pipedPF[k][1]);
                }
            }

            pipe_t pipedFN;
            
            if (pipe(pipedFN) < 0 )
            {	printf("Errore creazione pipe\n");
                exit(255);
            }

            int pidNipote;	// memorizzo il valore di ritorno della funzione fork
            
            if ((pidNipote = fork()) < 0)	/* Il processo padre crea un figlio */
            {	/* In questo caso la fork e' fallita */
                printf("Errore durante la fork\n");
                exit(254);
            }
            
            if (pidNipote == 0)
            {	/* processo nipote */
                close(0);
                
                //controllo se il file e' accedibile
                int fd = 0; /*variabile che conterra il file descriptor del file che stiamo per aprire */
                if((fd = open(argv[i + 1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
                    printf("Errore in apertura file %s dato che fd = %d\n", argv[i + 1], fd);
                    exit(254);
                }

                close(1);
                dup(pipedFN[1]);
                close(pipedFN[0]);
                close(pipedFN[1]);

                execlp("wc", "wc", "-l", NULL); // in caso di lista, ricordati di 0 terminare la lista dei parametri!
                /* si esegue l'istruzione seguente SOLO in caso di fallimento */
                
                perror("errore esecuzione comando"); // 
                exit(errno);
            }
            /* processo figlio */

            close(pipedFN[1]);
            
            int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
            int ritorno = 1;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
            if ((pidNipote = wait(&status)) < 0) {
                printf("Non e' stato creato nessun processo figlio\n");
                exit(253);
            }
            
            if ((status & 0xFF) != 0)
            {
                printf("Il processo figlio è stato terminato in modo anomalo\n");
                exit(253);
            } else {
                ritorno = (status >> 8) & 0xFF;

                char c;
                char* line = malloc(1);
                int capacity = 1;
                int size = 0;
                
                while (read(pipedFN[0], &c, 1) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
                {
                    if (size >= capacity) {
                        capacity*=2;
                        line = realloc(line, sizeof(*line) * capacity);
                    }

                    if (c == '\n') {
                        line[size] = 0;
                        size++;
                        break;
                    }

                    line[size] = c;
                    size++;
                }

                int numeroLinee = atoi(line);
                printf("DEBUG:Il file %s ha %d linee\n", argv[i + 1], numeroLinee);
                write(pipedPF[i][1], &numeroLinee, sizeof(numeroLinee));
            }
            
            exit(ritorno);
        }
        /* processo padre */
        
    }

    //chiudo tutti i lati di scrittura delle pipe
    for(int k = 0; k < N; k++){
        close(pipedPF[k][1]);
    }

    long int sum = 0;

    for(int i = 0; i < N; i++) {
        int numeroLinee;
        read(pipedPF[i][0], &numeroLinee, sizeof(numeroLinee));

        printf("DEBUG:Il file %s ha %d linee\n", argv[i + 1], numeroLinee);

        sum += numeroLinee;
    }

    printf("La somma di tutte le linee dei file passati è %ld\n", sum);

    for (int i = 0; i < N; i++) {
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
            exit(4);
        } else {
            ritorno = (status >> 8) & 0xFF;
            printf("il figlio pid=%i ed ha ritornato=%i\n",pidFiglio,ritorno);
        }
    }

    exit(0);
}