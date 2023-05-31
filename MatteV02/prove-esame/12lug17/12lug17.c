#include <stdio.h>		// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>		// Includo la libreria per la funzione exit
#include <unistd.h>		// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe,
#include <fcntl.h>		// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <stdbool.h>


int mia_random(int n)
{
    int casuale;
    casuale = rand() % n;
    casuale++;
    return casuale;
}

struct struttura {
    int c1;
    int c2;
    char c3[250];
} typedef struttura;

//definisco il tipo pipe_t
typedef int pipe_t[2];

int main(int argc, char** argv) {
    if (argc < 2 + 1) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 2*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }

    int N = (argc - 1) / 2;

    if (N % 2 != 0) {
        printf("Errore, il processo si aspetta un numero pari di parametri\n"); 
        exit(1);
    }

    struttura s;
    
    int *X = malloc(sizeof(*X) * N);
    if (X == NULL) {
        printf("Errore nell'allocazione della memoria\n"); 
        exit(2);
    }

    for (int i = 0; i < N; i++)
    {
        //controllo utlizzando la funzione atoi se il argv[i*2+2] e un numero strettamente positivo
        if((X[i] = atoi(argv[i*2+2])) <= 0){
            printf("il parametro %s non un numero positivo\n",argv[i*2+2]);
            exit(2);
        }
    }
    

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

    for (int i = 0; i < N; i++)
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
                close(pipedFP[k][0]);
                if (k != i) {
                    close(pipedFP[k][1]);
                }
            }
            
            pipe_t pipedNF;
            
            if (pipe(pipedNF) < 0 )
            {	printf("Errore creazione pipe\n");
                exit(255);
            }

            int pidNipote;	// memorizza il valore di ritorno della funzione fork
            
            if ((pidNipote = fork()) < 0)	/* Il processo padre crea un figlio */
            {	/* In questo caso la fork e' fallita */
                printf("Errore durante la fork\n");
                exit(255);
            }
            
            if (pidNipote == 0)
            {	/* processo nipote */
                close(pipedNF[0]);

                close(1);
                dup(pipedNF[1]);
                close(pipedNF[1]);

                char parametro[11];
                srand(time(NULL));

                int random = mia_random(X[i]);

                sprintf(parametro, "-%d", random);

                write(1, &random, sizeof(random));

                execlp("head", "head", parametro, argv[i * 2 + 1], (char*)0); // in caso di lista, ricordati di 0 terminare la lista dei parametri! (char*)0
                /* si esegue l'istruzione seguente SOLO in caso di fallimento */
                
                perror("errore esecuzione comando");
                exit(errno);
            }
            /* processo figlio */
            
            close(pipedNF[1]);

            char c;	// carattere usato per leggere il contenuto del file
            int c3_size = 0;
            s.c1 = getpid();
            s.c2 = 0;

            int random;
            read(pipedNF[0], &random, sizeof(random));
            
            while (read(pipedNF[0], &c, sizeof(char)) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
            {
                if (c == '\n') {
                    s.c3[c3_size++] = 0;
                    s.c2++;
                    write(pipedFP[i][1], &s, sizeof(s));
                    c3_size = 0;
                } else {
                    s.c3[c3_size++] = c;
                }
            }

            if ((pidNipote = wait((int*) 0)) < 0) {
                printf("Non e' stato creato nessun processo figlio\n");
                exit(255);
            }

            exit(random);
        }
        /* processo padre */
        
    }

    bool continua = true;

    while (continua) {
        continua = false;
        for (int i = 0; i < N; i++)
        {
            if (read(pipedFP[i][0], &s, sizeof(s))) {
                continua = true;
                printf("Il padre ha ricevuto una struttura con c1=%d, c2=%d, c3=%s\n", s.c1, s.c2, s.c3);
            }
        }
        
    }
    
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
            printf("Il figlio di PID %d ha ritornato %d\n",pidFiglio,ritorno);
        }
    }
    exit(0);
}