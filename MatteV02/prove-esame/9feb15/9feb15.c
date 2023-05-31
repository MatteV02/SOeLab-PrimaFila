#include <stdio.h>		// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>		// Includo la libreria per la funzione exit
#include <unistd.h>		// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe,
#include <fcntl.h>		// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait


//definisco il tipo pipe_t
typedef int pipe_t[2];

int main(int argc, char** argv) {
    if (argc < 2 + 1) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 2*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }

    char linea[256];    // suppongo ogni linea sia lunga al massimo 256 caratteri
    int linea_size = 0;
    
    if ((argc - 1) % 2 != 0) {
        printf("Errore, il numero di parametri passato non e' pari\n"); 
        exit(1);
    }

    int N = (argc - 1) / 2;

    int* Xi = malloc(sizeof(*Xi) * N);
    if (Xi == NULL) {
        printf("Errore nell'allocazione della memoria\n"); 
        exit(2);
    }

    for (int i = 0; i < N; i++)
    {
        //controllo utlizzando la funzione atoi se il argv[2*i + 2] e un numero strettamente positivo
        if((Xi[i] = atoi(argv[2*i + 2])) <= 0){
            printf("il parametro %s non un numero positivo\n",argv[2*i + 2]);
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

    for (int i = 0; i < N; i++) {
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

                // chiudo lo stdout
                close(1);
                // duplico la pipe lato  scrittura
                dup(pipedNF[1]);
                // chiudo il lato della pipe duplicato
                close(pipedNF[1]);

                char parametro[12];
                sprintf(parametro, "-%d", Xi[i]);

                execlp("head", "head", parametro, argv[2*i + 1], (char*)0); // in caso di lista, ricordati di 0 terminare la lista dei parametri! (char*)0
                /* si esegue l'istruzione seguente SOLO in caso di fallimento */
                
                perror("errore esecuzione comando"); 
                exit(-1);
            }
            /* processo figlio */

            close(pipedNF[1]);

            char c;	// carattere usato per leggere il contenuto del file
            
            for (int nl = 0; nl < Xi[i]; nl++)
            {
                linea_size = 0;
                while (read(pipedNF[0], &c, sizeof(char)) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
                {
                    if (c == '\n') {
                        linea[linea_size++] = 0;
                        break;
                    }
                    linea[linea_size++] = c;
                }
            }

            printf("DEBUG:figlio %d manda stringa %s a padre\n", i, linea);

            write(pipedFP[i][1], linea, linea_size);


            int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
            int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
            if ((pidNipote = wait(&status)) < 0) {
                printf("Non e' stato creato nessun processo figlio\n");
                exit(255);
            }
            
            if ((status & 0xFF) != 0)
            {
                printf("Il processo figlio con PID %d è stato terminato in modo anomalo\n", pidFiglio);
            } else {
                ritorno = (status >> 8) & 0xFF;
            }
            
            exit(ritorno);
        }
        /* processo padre */
        
    }

    //chiudo tutte le pipe in lettura e scrittura
    for(int k = 0; k < N; k++){
        close(pipedFP[k][1]);
    }

    for (int i = 0; i < N; i++)
    {
        char c;	// carattere usato per leggere il contenuto del file
        
        while (read(pipedFP[i][0], &c, sizeof(char)) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
        {
            linea[linea_size++] = c;
        }

        printf("Il processo d'ordine %d ha estratto la %d-esima linea del file %s: %s\n", i, Xi[i], argv[2 * i + 1], linea);
        linea_size = 0;
    }

    for (int i = 0; i < N; i++)
    {
        int pidFiglio;
        int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
        int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
        if ((pidFiglio = wait(&status)) < 0) {
            printf("Non e' stato creato nessun processo figlio\n");
            exit(5);
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