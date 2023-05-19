#include <stdio.h>	// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close, fork, exec, read, write
#include <fcntl.h>	// Includo la libreria per la funzione open e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait

//definisco il tipo pipe_t
typedef int pipe_t[2];

struct lineaOrdinata {
    int pidNipote;
    int lunghezzaLinea;
    char linea[250];
} typedef lineaOrdinata;

int main(int argc, char** argv) {
    if (argc < 3 + 1) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 3*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }

    int N = argc - 1;

    //apro N pipes
    pipe_t* pipedPF=malloc(sizeof(pipe_t)*N);
    for(int i=0;i<N;i++){
        if(pipe(pipedPF[i])<0){
            printf("Errore nella creazione della pipe numero:%i",i);
            exit(2);
        }
    }

    for (int i = 0; i < N; i++) {
        int pidFiglio;	// memorizzo il valore di ritorno della funzione fork
        
        if ((pidFiglio = fork()) < 0)	/* Il processo padre crea un figlio */
        {	/* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(3);
        }
        
        if (pidFiglio == 0)
        {	/* processo figlio */
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
            
            if ((pidNipote = fork()) < 0)	/* Il processo figlio crea un nipote */
            {	/* In questo caso la fork e' fallita */
                printf("Errore durante la fork\n");
                exit(254);
            }
            
            if (pidNipote == 0)
            {	/* processo nipote */
                close(pipedFN[0]);

                //controllo se il file e' accedibile
                int fd = 0; /*variabile che conterra il file descriptor del file che stiamo per aprire */
                if((fd = open(argv[i + 1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
                    printf("Errore in apertura file %s dato che fd = %d\n", argv[i + 1], fd);
                    exit(255);
                }

                close(1);
                dup(pipedFN[1]);
                close(pipedFN[1]);

                execlp("sort", "sort", "-f", argv[i + 1], NULL);

                exit(254);
            }
            /* processo figlio */

            close(pipedFN[1]);


            char buffer;
            
            lineaOrdinata linea = { pidNipote, 0, "" };
            
            while (read(pipedFN[0], &buffer, 1) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
            {
                // suppongo la lunghezza massima di una linea sia 250 caratteri

                linea.linea[linea.lunghezzaLinea++] = buffer;

                if (buffer == '\n') {
                    break;
                }
            }

            write(pipedPF[i][1], &linea, sizeof(linea));

            exit(linea.lunghezzaLinea - 1);
        }
        /* processo padre */
        
    }

    //chiudo tutte le pipe in scrittura
    for(int k = 0; k < N; k++){
        close(pipedPF[k][1]);
    }

    for (int i = 0; i < N; i++) {
        lineaOrdinata linea;
        if (read(pipedPF[i][0], &linea, sizeof(linea)) != sizeof(linea)) {
            printf("Errore in lettura della pipe %d-esima\n", i);
            continue;
        }

        linea.linea[linea.lunghezzaLinea - 1] = 0;

        printf("Il file di nome %s è stato trattato dal figlio di PID %d, ha una prima linea ordinata lunga %d dal seguente contenuto:\n%s\n\n", argv[i + 1], linea.pidNipote, linea.lunghezzaLinea, linea.linea);
    }

    for (int i = 0; i < N; i++) {
        int pidFiglio;
        int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
        int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
        if ((pidFiglio = wait(&status)) < 0) {
            printf("Errore nella wait dell'%d-esimo figlio\n", i);
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