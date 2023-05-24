#include <stdio.h>	// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close, fork, famiglia exec, read, write, pipe
#include <sys/wait.h>	// Includo la libreria per la funzione wait


//definisco il tipo pipe_t
typedef int pipe_t[2];

int main(int argc, char** argv) {
    char c;     // variabile per memorizzare un singolo carattere
    char linea[256];    // suppongo ogni linea del file sia lunga al massimo 256 caratteri
    int N = argc - 1;   // numero di parametri passati
    int pidFiglio;	// memorizzo il valore di ritorno della funzione fork e della funzione wait
    int linea_size = 0; // variabile che contiene la lunghezza della variabile "linea"
    int status;	// La variabile usata per memorizzare quanto ritornato per indirizzo dalla primitiva wait 
    int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
    
    if (argc < 1 + 1) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 1*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }

    //apro N pipes
    pipe_t* pipedFP = (pipe_t*)malloc(sizeof(pipe_t) * (N));    // questa variabile conterrà le pipe per la comunicazione da figli a padre
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

    for (int i = 0; i < N; i++) 
    {       
        if ((pidFiglio = fork()) < 0)	/* Il processo padre crea un figlio */
        {	/* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(3);
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

            // redireziono lo standard output sulla pipe
            close(1);
            dup(pipedFP[i][1]);
            close(pipedFP[i][1]);

            execlp("head", "head", "-1", argv[i + 1]);

            // eseguo questo solo se la execlp fallisce
            exit(255);
        }
        /* processo padre */
        
    }

    //chiudo tutte le pipe in scrittura
    for(int k = 0; k < N; k++){
        close(pipedFP[k][1]);
    }

    //scorro ogni pipe di ogni processo figlio
    for (int i = 0; i < N; i++) 
    {        
        //leggo ogni riga e la scrivo sull'array "linea"
        while (read(pipedFP[i][0], &c, 1) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
        {
            linea[linea_size++] = c;
            if (c == '\n') {
                linea[linea_size - 1] = 0;  //termino la stringa
                printf("Il processo figlio di indice %d ha letto dal file \"%s\" la linea \"%s\"\n", i, argv[i + 1], linea);
                linea_size = 0;
                break;
            }
        }
    }

    //faccio N wait
    for (int i = 0; i < N; i++) {
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