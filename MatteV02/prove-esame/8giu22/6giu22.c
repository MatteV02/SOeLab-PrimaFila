#include <stdio.h>		// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>		// Includo la libreria per la funzione exit
#include <unistd.h>		// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe,
#include <fcntl.h>		// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <errno.h>
#define MSG_SIZE 69


//definisco il tipo pipe_t
typedef int pipe_t[2];

int main(int argc, char** argv) {
    int N = argc - 1;   // numero di file passati per parametro
    pipe_t* pipedFigli; // vettore di pipe per la comunicazione da figlio-0 agli altri figli
    char linea[MSG_SIZE];   // array per leggere MSG_SIZE byte da file
    char buffer[MSG_SIZE];  // array per leggere i messaggi mandati dal figlio-0
    int pidFiglio;	// memorizza il valore di ritorno della funzione fork e wait per quanto concerne i processi figli
    int fd = 0; // variabile che conterra' il file descriptor dei file che verranno aperti
    int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
    int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio


    if (argc < 3 + 1) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 3*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }

    //apro N - 1 pipes
    pipedFigli = (pipe_t*)malloc(sizeof(pipe_t) * (N - 1));
    if (pipedFigli == NULL) {
        printf("Errore nell'allocazione della memoria\n");
        exit(2);
    }
    
    for(int i = 0; i < N - 1; i++){
        if(pipe(pipedFigli[i])<0){
            printf("Errore nella creazione della pipe numero:%d\n",i);
            exit(2);
        }
    }

    for (int n = 0; n < N; n++)
    {        
        if ((pidFiglio = fork()) < 0)	/* Il processo padre crea un figlio */
        {	/* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(3);
        }
        
        if (pidFiglio == 0)
        {	/* processo figlio */
            int nNipoti;    // variabile che memorizza il numero di nipoti che questo processo figlio generera'
            int pidNipote;	// memorizza il valore di ritorno della funzione fork e wait rispetto ai processi nipoti

            if (n == 0)
            {
                for (int k = 0; k < N - 1; k++)
                {
                    close(pipedFigli[k][0]);
                }
                
                //apro il file argv[1]
                if((fd = open(argv[1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire il file */
                    printf("Errore in apertura file %s dato che fd = %d\n", argv[1], fd);
                    exit(255);
                }

                while (read(fd, linea, MSG_SIZE) > 0)
                {
                    linea[MSG_SIZE - 1] = 0;
                    // printf("DEBUG:mando ai figli la linea %s\n", linea);
                    for (int i = 0; i < N - 1; i++)
                    {
                        write(pipedFigli[i][1], &linea, sizeof(linea));
                    }
                }
                
                exit(n);
            }
            
            //chiudo tutte le pipe in lettura e scrittura tranne la n-esima pipe, aperta in lettura
            for(int k = 0; k < N - 1; k++){
                close(pipedFigli[k][1]);
                if (k != n - 1) {
                    close(pipedFigli[k][0]);
                }
            }

            //apro il file argv[n + 1]
            if((fd = open(argv[n + 1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire il file */
                printf("Errore in apertura file %s dato che fd = %d\n", argv[n + 1], fd);
                exit(255);
            }

            nNipoti = lseek(fd, 0, SEEK_END) / MSG_SIZE;
            // printf("DEBUG:il figlio %d ha %d nipoti\n", n, nNipoti);

            while (read(pipedFigli[n - 1][0], &buffer, MSG_SIZE) > 0) {
                // printf("DEBUG:figlio %d, entro nel while\n", n);
                lseek(fd, 0, SEEK_SET);
                for (int i = 0; i < nNipoti; i++)
                {
                    read(fd, &linea, MSG_SIZE);
                    linea[MSG_SIZE - 1] = 0;

                    // printf("DEBUG:creo processo nipote\n");
                    
                    if ((pidNipote = fork()) < 0)	/* Il processo padre crea un figlio */
                    {	/* In questo caso la fork e' fallita */
                        printf("Errore durante la fork\n");
                        exit(255);
                    }
                    
                    if (pidNipote == 0)
                    {	/* processo nipote */

                        close(1);
                        //apro il file "/dev/null"
                        int fd = 0; // variabile che conterra' il file descriptor del file "/dev/null"
                        if((fd = open("/dev/null", O_WRONLY)) < 0){		/* ERRORE se non si riesce ad aprire il file */
                            printf("Errore in apertura file %s dato che fd = %d\n", "/dev/null", fd);
                            exit(255);
                        }

                        execlp("diff", "diff", buffer, linea); // in caso di lista, ricordati di 0 terminare la lista dei parametri! (char*)0
                        /* si esegue l'istruzione seguente SOLO in caso di fallimento */
                        
                        perror("errore esecuzione comando");
                        exit(errno);

                        exit(0);
                    }
                    /* processo figlio */
                    
                    if ((pidNipote = wait(&status)) < 0) {
                        printf("Non e' stato creato nessun processo nipte\n");
                        exit(255);
                    }
                    
                    if ((status & 0xFF) != 0)
                    {
                        printf("Il processo nipote è stato terminato in modo anomalo\n");
                    } else {
                        ritorno = (status >> 8) & 0xFF;
                        // printf("DEBUG:confrontati i file %s e %s, ritorno = %d\n", buffer, linea, ritorno);
                        if (ritorno == 0) {
                            printf("I file %s e %s sono uguali\n", buffer, linea);
                        }
                    }
                }
            }

            exit(n);
        }
        /* processo padre */
        
    }

    //chiudo tutte le pipe in lettura e scrittura
    for(int k = 0; k < N; k++){
        close(pipedFigli[k][1]);
        close(pipedFigli[k][0]);
    }
    
    for (int n = 0; n < N; n++)
    {
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