#include <stdio.h>	// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe,
#include <fcntl.h>	// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <ctype.h>
#include <stdbool.h>

#define TRASFORMA_MINUSCOLE 0
#define TRASFORMA_NUMERI 1 

/*
    La funzione apre il file specificato e compie il tipo di trasformazione specificato
*/
void trasforma(char* file, int tipoTrasformazione, long int *nTrasformazioni) {
    //controllo se il file e' accedibile
    int fd = 0; /*variabile che conterra il file descriptor del file che stiamo per aprire */
    if((fd = open(file, O_RDWR)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
        printf("Errore in apertura file %s dato che fd = %d\n", file, fd);
        exit(255);
    }

    char c;
    
    while (read(fd, &c, sizeof(char)) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
    {
        printf("DEBUG:Leggo carattere %c dal file %s\n", c, file);
        bool trasforma = false;
        switch (tipoTrasformazione)
        {
        case TRASFORMA_MINUSCOLE:
            if (c >= 'a' && c <= 'z') {
                c -= ('a' - 'A');
                trasforma = true;
            }
            break;
        
        case TRASFORMA_NUMERI:
            if (isdigit(c))
            {
                trasforma = true;
                c = ' ';
            }
            break;
        }
        if (trasforma) {
            (*nTrasformazioni)++;
            printf("DEBUG:trasformo il carattere\n");
            lseek(fd, -1, SEEK_CUR);
            write(fd, &c, sizeof(c));
        }
    }
}

//definisco il tipo pipe_t
typedef int pipe_t[2];

int main(int argc, char** argv) {
    if (argc < 2 + 1) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 2*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }

    int N = argc - 1;
    long int nTrasformazioni = 0;

    //apro N pipes
    pipe_t* pipedFP = (pipe_t*)malloc(sizeof(pipe_t) * (N));
    if (pipedFP == NULL) {
        printf("Errore nell'allocazione della memoria\n");
        exit(2);
    }
    
    for(int i = 0;i < N;i++){
        if(pipe(pipedFP[i])<0){
            printf("Errore nella creazione della pipe numero:%i\n",i);
            exit(2);
        }
    }

    //apro N pipes
    pipe_t* pipedNP = (pipe_t*)malloc(sizeof(pipe_t) * (N));
    if (pipedNP == NULL) {
        printf("Errore nell'allocazione della memoria\n");
        exit(3);
    }
    
    for(int i = 0;i < N;i++){
        if(pipe(pipedNP[i])<0){
            printf("Errore nella creazione della pipe numero:%i\n",i);
            exit(3);
        }
    }

    for (int i = 0; i < N; ++i) {
        int pidFiglio;	// memorizzo il valore di ritorno della funzione fork
        
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

            int pidNipote;	// memorizzo il valore di ritorno della funzione fork
            
            if ((pidNipote = fork()) < 0)	/* Il processo padre crea un figlio */
            {	/* In questo caso la fork e' fallita */
                printf("Errore durante la fork\n");
                exit(255);
            }
            
            if (pidNipote == 0)
            {	/* processo nipote */
                //chiudo tutte le pipe in lettura e scrittura tranne la i-esima pipe, aperta in scrittura
                for(int k = 0; k < N; k++){
                    close(pipedNP[k][0]);
                    if (k != i) {
                        close(pipedNP[k][1]);
                    }
                }
                
                close(pipedFP[i][1]);   // chiudo l'ultima pipe rimasta dal processo figlio

                trasforma(argv[i + 1], TRASFORMA_MINUSCOLE, &nTrasformazioni);
                write(pipedNP[i][1], &nTrasformazioni, sizeof(nTrasformazioni));


                exit(nTrasformazioni / 256);
            }
            /* processo figlio */

            //chiudo tutte le pipe in lettura e scrittura del processo nipote
            for(int k = 0; k < N; k++){
                close(pipedNP[k][1]);
                close(pipedNP[k][0]);
            }

            trasforma(argv[i + 1], TRASFORMA_NUMERI, &nTrasformazioni);
            write(pipedFP[i][1], &nTrasformazioni, sizeof(nTrasformazioni));

            int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
            int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
            if ((pidNipote = wait(&status)) < 0) {
                printf("Non e' stato creato nessun processo figlio\n");
                exit(-1);
            }
            
            if ((status & 0xFF) != 0)
            {
                printf("Il processo figlio è stato terminato in modo anomalo\n");
            } else {
                ritorno = (status >> 8) & 0xFF;
                printf("il nipote pid=%i ed ha ritornato=%i\n",pidNipote,ritorno);
            }

            exit(nTrasformazioni / 256);
        }
        /* processo padre */
        
    }

    //chiudo tutti i lati di scrittura delle pipe del nipote e del figlio
    for(int k = 0; k < N; k++){
        close(pipedNP[k][1]);
        close(pipedFP[k][1]);
    }

    for (int i = 0; i < N; i++) {
        if (read(pipedFP[i][0], &nTrasformazioni, sizeof(nTrasformazioni)) == 0) {
            printf("Errore, pipe %d del figlio chiusa\n", i); 
            exit(5);
        }

        printf("Il figlio di ordine %d ha compiuto %ld trasformazioni sul file \"%s\"\n", i, nTrasformazioni, argv[i + 1]);

        if (read(pipedNP[i][0], &nTrasformazioni, sizeof(nTrasformazioni)) == 0) {
            printf("Errore, pipe %d del nipote chiusa\n", i); 
            exit(6);
        }

        printf("Il nipote di ordine %d ha compiuto %ld trasformazioni sul file \"%s\"\n", i, nTrasformazioni, argv[i + 1]);
    }

    for (int i = 0; i < N; i++)
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
            printf("Il processo figlio è stato terminato in modo anomalo\n");
        } else {
            ritorno = (status >> 8) & 0xFF;
            printf("il figlio pid=%i ed ha ritornato=%i\n",pidFiglio,ritorno);
        }
    }

    exit(0);
}