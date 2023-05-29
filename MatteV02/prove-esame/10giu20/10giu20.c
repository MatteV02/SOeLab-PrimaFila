#include <stdio.h>		// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>		// Includo la libreria per la funzione exit
#include <unistd.h>		// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe,
#include <fcntl.h>		// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <stdbool.h>
#include <ctype.h>


//definisco il tipo pipe_t
typedef int pipe_t[2];

int main(int argc, char** argv) {
    if (argc < 2 + 1) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 2*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }

    //creo il file "Camilla"
    int fd = 0; // variabile che conterra il file descriptor del file "Camilla" */
    if((fd = creat("Camilla", 0644)) < 0){		/* ERRORE se non si riesce a creare il file */
        printf("Errore in creazione del file %s dato che fd = %d\n", "Camilla", fd);
        exit(2);
    }

    int Q = argc - 1;
    char linea[251];    // suppongo le linee su file siano lunghe al massimo 250 caratteri
    int linea_size = 0;

    //apro Q pipes
    pipe_t* pipedFP = (pipe_t*)malloc(sizeof(pipe_t) * (Q));
    if (pipedFP == NULL) {
        printf("Errore nell'allocazione della memoria\n");
        exit(3);
    }
    
    for(int q = 0; q < Q; q++){
        if(pipe(pipedFP[q])<0){
            printf("Errore nella creazione della pipe numero:%d\n",q);
            exit(3);
        }
    }

    for (int q = 0; q < Q; q++)
    {
        int pidFiglio;	// memorizza il valore di ritorno della funzione fork
        
        if ((pidFiglio = fork()) < 0)	/* Il processo padre crea un figlio */
        {	/* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(4);
        }
        
        if (pidFiglio == 0)
        {	/* processo figlio */
            //chiudo tutte le pipe in lettura e scrittura tranne la q-esima pipe, aperta in scrittura
            for(int k = 0; k < Q; k++){
                close(pipedFP[k][0]);
                if (k != q) {
                    close(pipedFP[k][1]);
                }
            }

            //apro il file argv[q + 1]
            int fd = 0; // variabile che conterra' il file descriptor del file argv[q + 1]
            if((fd = open(argv[q + 1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire il file */
                printf("Errore in apertura file %s dato che fd = %d\n", argv[q + 1], fd);
                exit(255);
            }

            char c;	// carattere usato per leggere il contenuto del file
            int lineeTrovate = 0;
            
            while (read(fd, &c, sizeof(char)) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
            {
                linea[linea_size++] = c;
                if (c == '\n') {
                    if (linea_size < 10 && isdigit(linea[0])) {
                        write(pipedFP[q][1], &linea, linea_size);
                        lineeTrovate++;
                    }
                    linea_size = 0;
                }
            }

            exit(lineeTrovate);
        }
        /* processo padre */
        
    }

    //chiudo tutte le pipe in scrittura
    for(int k = 0; k < Q; k++){
        close(pipedFP[k][1]);
    }

    bool continua = true;

    while (continua)
    {
        continua = false;
        for (int q = 0; q < Q; q++)
        {
            char c;	// carattere usato per leggere il contenuto del file
            
            while (read(pipedFP[q][0], &c, sizeof(char)) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
            {
                continua = true;

                if (c == '\n') {
                    linea[linea_size++] = 0;
                    break;
                } else {
                    linea[linea_size++] = c;
                }
            }
            printf("DEBUG:figlio %d, padre legge %d caratteri\n", q, linea_size);
            if (linea_size > 0)
            {
                printf("L'indice del processo figlio e' %i, la linea trovata corrispondente alle specifiche e' \"%s\"\n", q, linea);
            }

            linea_size = 0;
            
        }
    }
    
    for (int i = 0; i < Q; i++)
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
            printf("Il processo figlio è stato terminato in modo anomalo\n");
        } else {
            ritorno = (status >> 8) & 0xFF;
            printf("Il figlio di PID %d ha ritornato %d\n",pidFiglio,ritorno);
        }
    }
    
    
    

    exit(0);
}