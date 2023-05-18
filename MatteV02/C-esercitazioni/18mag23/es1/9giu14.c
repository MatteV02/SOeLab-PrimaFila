#include <stdio.h>	// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close, fork, exec, read, write
#include <fcntl.h>	// Includo la libreria per la funzione open e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait

//definisco il tipo pipe_t
typedef int pipe_t[2];

struct returnValue {
    int index;
    int ret;
} typedef returnValue;

int main(int argc, char** argv) {
    if (argc < 2 + 1) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 2*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }

    int N = argc - 1;

    int *returns = malloc(N * sizeof(*returns));

    pipe_t piped_figli;
    
    if (pipe(piped_figli) < 0 )
    {	printf("Errore creazione pipe\n");
        exit(2);
    }    

    for (int i = 0; i < N; i++)
    {
        int pidChild;	// memorizzo il valore di ritorno della funzione fork
        
        if ((pidChild = fork()) < 0)	/* Il processo padre crea un figlio */
        {	/* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(3);
        }
        
        if (pidChild == 0)
        {
            close(piped_figli[0]);

            pipe_t piped_nipoti;
            
            if (pipe(piped_nipoti) < 0 )
            {	printf("Errore creazione pipe\n");
                exit(4);
            }
            
            int pidNephew;	// memorizzo il valore di ritorno della funzione fork
            
            if ((pidNephew = fork()) < 0)	/* Il processo padre crea un figlio */
            {	/* In questo caso la fork e' fallita */
                printf("Errore durante la fork\n");
                exit(6);
            }
            
            if (pidNephew == 0)
            {	/* processo nipote */
                close(piped_nipoti[0]);

                close(0);
                //controllo se il file e' accedibile
                int fd = 0; /*variabile che conterra il file descriptor del file che stiamo per aprire */
                if((fd = open(argv[i + 1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
                    printf("Errore in apertura file %s dato che fd = %d\n", argv[i + 1], fd);
                    exit(5);
                }

                close(1);
                dup(piped_nipoti[1]);
                close(piped_nipoti[1]);

                execlp("wc", "wc", "-l", NULL);

                exit(255);
            }
            /* processo figlio */

            close(piped_nipoti[1]);
            

            int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
            int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
            if ((pidNephew = wait(&status)) < 0) {
                printf("Non e' stato creato nessun processo nipote\n");
                exit(7);
            }
            
            if ((status & 0xFF) != 0)
            {
                printf("Il processo nipote è stato terminato in modo anomalo\n");
                exit(7);
            } else {
                ritorno = (status >> 8) & 0xFF;
                if (ritorno == 255) {
                    printf("Errore nella wc del nipote %i", i);
                } else {
                    char c;
                    char* line = malloc(1);
                    int capacity = 1;
                    int size = 0;
                    
                    while (read(piped_nipoti[0], &c, 1) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
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

                    int numero_linee = atoi(line);
                    printf("DEBUG: numero linee file %s = %d\n", argv[i + 1], numero_linee);

                    returnValue ret = {i, numero_linee};

                    write(piped_figli[1], &ret, sizeof(ret));
                }
            }

            exit(0);
        }
        /* processo padre */
        
    }
    
    close(piped_figli[1]);

    long int sum = 0;

    for (int i = 0; i < N; i++)
    {
        returnValue ret;

        read(piped_figli[0], &ret, sizeof(ret));

        returns[ret.index] = ret.ret;

        sum += ret.ret;
    }

    printf("La somma delle linee dei file e' %ld\n", sum);
    
    for (int i = 0; i < N; i++) {
        int pidChild;
        int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
        int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
        if ((pidChild = wait(&status)) < 0) {
            printf("Non e' stato creato nessun processo figlio\n");
            exit(8);
        }
        
        if ((status & 0xFF) != 0)
        {
            printf("Il processo figlio è stato terminato in modo anomalo\n");
            exit(8);
        } else {
            ritorno = (status >> 8) & 0xFF;
            printf("il figlio pid=%i ed ha ritornato=%i\n",pidChild,ritorno);
        }
    }

    exit(0);
}