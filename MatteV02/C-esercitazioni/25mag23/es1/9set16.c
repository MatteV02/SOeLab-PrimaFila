#include <stdio.h>	// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe,
#include <fcntl.h>	// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <stdbool.h>

struct struttura
{
    char v1;
    long int v2;
} typedef struttura;

void scambia(struttura* s1, struttura* s2) {
    struttura temp = {s1->v1, s1->v2};
    (*s1) = *s2;
    (*s2) = temp;
}

void bubbleSort(struttura v[], int dim)
{ 
    int i; bool ordinato = false;
    while (dim>1 && !ordinato)
    { 
        ordinato = true; /* hp: è ordinato */
        for (i=0; i<dim-1; i++)
            if (v[i].v2>v[i+1].v2)
            {
                scambia(&v[i],&v[i+1]);
                ordinato = false;
            }
        dim--;
    }
}

//definisco il tipo pipe_t
typedef int pipe_t[2];

int main(int argc, char** argv) {
    struttura strutture[26] = {};

    if (argc != 1 + 1) /* controllo sul numero di parametri: devono essere in numero uguale a 1*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }

    //apro 26 pipes
    pipe_t* piped = (pipe_t*)malloc(sizeof(pipe_t) * (26));
    if (piped == NULL) {
        printf("Errore nell'allocazione della memoria\n");
        exit(2);
    }
    
    for(int i = 0;i < 26;i++){
        if(pipe(piped[i])<0){
            printf("Errore nella creazione della pipe numero:%i\n",i);
            exit(2);
        }
    }

    for (int i = 0; i < 26; i++)
    {
        int pidFiglio;	// memorizzo il valore di ritorno della funzione fork
        
        if ((pidFiglio = fork()) < 0)	/* Il processo padre crea un figlio */
        {	/* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(3);
        }
        
        if (pidFiglio == 0)
        {	/* processo figlio */
            char c = 'a' + (char)i;

            // chiudo tutte le pipe tranne il lato di lettura della pipe i e il lato di scrittura della pipe (i + 1) % 26
            for(int k = 0; k < 26; k++){
                if (k != i) {
                    close(piped[k][0]);
                    close(piped[(k + 1) % 26][1]);
                }
            }

            if (i != 0) {
                read(piped[i][0], &strutture, sizeof(strutture));
            }

            //controllo se il file e' accedibile
            int fd = 0; /*variabile che conterra il file descriptor del file che stiamo per aprire */
            if((fd = open(argv[1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
                printf("Errore in apertura file %s dato che fd = %d\n", argv[1], fd);
                exit(255);
            }

            char cf;
            
            while (read(fd, &cf, sizeof(char)) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
            {
                if (cf == c) {
                    strutture[i].v2++;
                }
            }

            strutture[i].v1 = c;
            write(piped[(i + 1) % 26][1], &strutture, sizeof(strutture));

            exit(cf);
        }
        /* processo padre */
        
    }

    //chiudo tutte le pipe in lettura e scrittura tranne la 0-esima pipe, aperta in  lettura
    for(int k = 0; k < 26; k++){
        close(piped[k][1]);
        if (k != 0) {
            close(piped[k][0]);
        }
    }

    read(piped[0][0], &strutture, sizeof(strutture));

    bubbleSort(strutture, 26);

    printf("Stampo le strutture ordinate:\n");
    for (int i = 0; i < 26; i++)
    {
        printf("v1: %c, v2: %ld\n", strutture[i].v1, strutture[i].v2);
    }
    
    for (int i = 0; i < 26; i++)
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