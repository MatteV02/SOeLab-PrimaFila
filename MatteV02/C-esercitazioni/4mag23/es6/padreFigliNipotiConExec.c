#include <stdio.h>	// Includo la libreria per la funzione printf
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close, fork e exec
#include <fcntl.h>	// Includo la libreria per la funzione open e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <string.h>
#include <errno.h>

#define PERM 0640


int main(int argc, char** argv) {
    if (argc < 4) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 3*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }

    for (int i = 0; i < argc -1; i++)
    {
        int pidChild;	// memorizzo il valore di ritorno della funzione fork
        
        if ((pidChild = fork()) < 0)	/* Il processo padre crea un figlio */
        {	/* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(2);
        }
        
        if (pidChild == 0)
        {	/* processo figlio */
            char string[256] = "";
            strcat(string, argv[i + 1]);
            strcat(string, ".sort");

            close(1);
            int FOut = open(string, O_WRONLY | O_CREAT | O_TRUNC, PERM);

            if (FOut < 0) {
                perror("Errore, non sono riuscito a creare il file\n"); // #include <errno.h>
                exit(errno);
            }

            int pidNipote;	// memorizzo il valore di ritorno della funzione fork
            
            if ((pidNipote = fork()) < 0)	/* Il processo padre crea un figlio */
            {	/* In questo caso la fork e' fallita */
                printf("Errore durante la fork\n");
                exit(3);
            }
            
            if (pidNipote == 0)
            {	/* processo figlio */
                close(0);
                open(argv[i + 1], O_RDONLY);

                execlp("sort", "sort", NULL); // in caso di lista, ricordati di 0 terminare la lista dei parametri!
                /* si esegue l'istruzione seguente SOLO in caso di fallimento */
                
                perror("errore esecuzione comando"); // #include <errno.h>
                exit(errno);
            }
            /* processo padre */
            
            int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
            int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
            if ((pidNipote = wait(&status)) < 0) {
                printf("Non e' stato creato nessun processo figlio\n");
                exit(4);
            }
            
            if ((status & 0xFF) != 0)
            {
                printf("Il processo figlio è stato terminato in modo anomalo\n");
                exit(4);
            } else {
                ritorno = (status >> 8) & 0xFF;
            }

            exit(ritorno);
        }
        /* processo padre */

    }

    while (1) {
        int pidChild;
        int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
        int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
        if ((pidChild = wait(&status)) < 0) {
            break;
        }
        
        if ((status & 0xFF) != 0)
        {
            printf("Il processo figlio è stato terminato in modo anomalo\n");
            exit(5);
        } else {
            ritorno = (status >> 8) & 0xFF;
        }
        
        printf("Il processo figlio ha PID %d, valore di ritorno %d\n", pidChild, ritorno);
    }
    

    exit(0);
}