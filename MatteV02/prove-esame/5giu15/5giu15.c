#include <stdio.h>	// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe,
#include <fcntl.h>	// Includo la libreria per la funzione open, creat e le relative macro
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

    int M = argc - 1;

    //apro N pipes
    pipe_t* pipedFP = (pipe_t*)malloc(sizeof(pipe_t) * (M));
    if (pipedFP == NULL) {
        printf("Errore nell'allocazione della memoria\n");
        exit(2);
    }
    
    for(int i = 0;i < M;i++){
        if(pipe(pipedFP[i])<0){
            printf("Errore nella creazione della pipe numero:%i\n",i);
            exit(2);
        }
    }

    for (int i = 0; i < M; i++)
    {
        int pidFiglio;	// memorizzo il valore di ritorno della funzione fork
        
        if ((pidFiglio = fork()) < 0)	/* Il processo padre crea un figlio */
        {	/* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(3);
        }
        
        if (pidFiglio == 0)
        {	/* processo figlio */
            //chiudo tutte le pipe in lettura e scrittura tranne la i-esima pipe, aperta in scrittura
            for(int k = 0; k < M; k++){
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

            int pidNipote;	// memorizzo il valore di ritorno della funzione fork
            
            if ((pidNipote = fork()) < 0)	/* Il processo padre crea un figlio */
            {	/* In questo caso la fork e' fallita */
                printf("Errore durante la fork\n");
                exit(254);
            }
            
            if (pidNipote == 0)
            {	/* processo nipote */
                close(pipedFP[i][1]);
                close(pipedNF[0]);

                close(1);
                dup(pipedNF[1]);
                close(pipedNF[1]);

                execlp("tail", "tail", "-1", argv[i + 1]); // in caso di lista, ricordati di 0 terminare la lista dei parametri!
                /* si esegue l'istruzione seguente SOLO in caso di fallimento */
                
                perror("errore esecuzione tail"); 
                exit(errno);
            }
            /* processo figlio */

            close(pipedNF[1]);
            
            char c;
            int lunghezzaLinea = 0;
            
            while (read(pipedNF[0], &c, sizeof(char)) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
            {
                if (c == '\n') {
                    break;
                }
                lunghezzaLinea++;
            }

            write(pipedFP[i][1], &lunghezzaLinea, sizeof(lunghezzaLinea));

            exit(0);
        }
        /* processo padre */
        
    }
    
    //chiudo tutte le pipe in lettura e scrittura tranne la i-esima pipe, aperta in scrittura
    for(int k = 0; k < M; k++){
        close(pipedFP[k][1]);
    }

    for (int i = M - 1; i >= 0; i--)
    {
        int lunghezzaLinea;

        read(pipedFP[i][0], &lunghezzaLinea, sizeof(lunghezzaLinea));

        printf("Il processo figlio %d che tratta il file %s ha l'ultima linea lunga %d\n", i, argv[i + 1], lunghezzaLinea);
    }

    for (int i = 0; i < M; i++)
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
            printf("il figlio pid=%i ed ha ritornato=%i\n",pidFiglio,ritorno);
        }
    }

    exit(0);
}