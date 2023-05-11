#include <stdio.h>	// Includo la libreria per la funzione printf
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close, fork e exec
#include <fcntl.h>	// Includo la libreria per la funzione open e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <ctype.h>
#include <stdbool.h>

//definisco il tipo pipe_t
typedef int pipe_t[2];


int main(int argc, char** argv) {
    if (argc < 3) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 2*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }

    int N = argc - 1;

    //apro N pipes
    pipe_t* piped=malloc(sizeof(pipe_t)*2);
    for(int i=0;i<2;i++){
        if(pipe(piped[i])<0){
            printf("Errore nella creazione della pipe numero:%i",i);
        }
    }

    for (int i = 1; i <= N; i++) {
        int pidChild;	// memorizzo il valore di ritorno della funzione fork
        
        if ((pidChild = fork()) < 0)	/* Il processo padre crea un figlio */
        {	/* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(2);
        }
        
        if (pidChild == 0)
        {	/* processo figlio */
            //controllo se il file e' accedibile
            int fd = 0; /*variabile che conterra il file descriptor del file che stiamo per aprire */
            if((fd = open(argv[i], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
                printf("Errore in apertura file %s dato che fd = %d\n nel processo %d", argv[i], fd, i);
                exit(3);
            }

            char c;
            int pipe_scrittura;

            //chiudo tutte le pipe in lettura
            close(piped[0][0]);
            close(piped[1][0]);
            if (i % 2 == 0) {
                close(piped[1][1]);
                pipe_scrittura = piped[0][1];
            } else {
                close(piped[0][1]);
                pipe_scrittura = piped[1][1];
            }
            
            while (read(fd, &c, 1) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
            {
                if (i % 2 == 0) {
                    if (isdigit(c)) {
                        write(pipe_scrittura, &c, 1);

                    }
                } else {
                    if (isalpha(c)) {
                        write(pipe_scrittura, &c, 1);
                    }
                }
                
            }
            
            exit((int)c);
        }
        /* processo padre */
        
    }

    close(piped[0][1]);
    close(piped[1][1]);

    bool lettodispari = true, lettopari = true;

    while (lettodispari || lettopari) {
        char c;

        if (read(piped[1][0], &c, 1) > 0) {
            printf("%c", c);
        } else {
            lettodispari = false;
        }

        if (read(piped[0][0], &c, 1) > 0) {
            printf("%c", c);
        } else {
            lettopari = false;
        }
    }

    for (int i = 0; i < N; i++) {
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