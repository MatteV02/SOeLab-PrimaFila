#include <stdio.h>    // Includo la libreria per la funzione printf
#include <stdlib.h>   // Includo la libreria per la funzione exit
#include <unistd.h>   // Includo la libreria per la funzione close e fork
#include <fcntl.h>    // Includo la libreria per la funzione open e le relative macro
#include <sys/wait.h> // Includo la libreria per la funzione wait
#include <string.h>

int main(int argc, char **argv)
{
    if (argc < 4) /* controllo sul numero di parametri: devono essere in numero maggiore a 3*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }

    int N = argc - 2;

    if (strlen(argv[argc - 1]) != 1) {	// RICORDATI #include <string.h>
        printf("Errore, la stringa %s non e' un singolo carattere\n", argv[argc - 1]);
        exit(3);
    }
    char Cx = argv[argc - 1][0];

    for (int i = 0; i < N; i++)
    {
        int pidChild;	// memorizzo il valore di ritorno della funzione fork
        
        if ((pidChild = fork()) < 0)	/* Il processo padre crea un figlio */
        {	/* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(4);
        }
        
        if (pidChild == 0)
        {	/* processo figlio */
            //controllo se il file e' accedibile
            printf("DEBUG:Processo figlio %d\n", i + 1);
            int fd = 0; /*variabile che conterra il file descriptor del file che stiamo per aprire */
            printf("DEBUG:apro file %s\n", argv[i + 1]);
            if((fd = open(argv[i + 1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
                printf("Errore in apertura file %s dato che fd = %d\n", argv[i + 1], fd);
                exit(-1);
            }

            char buffer;
            int count = 0;
            
            while (read(fd, &buffer, 1) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
            {
                if (buffer == Cx) {
                    count++;
                }
            }

            printf("DEBUG:Contati %d caratteri\n", count);
            exit(count);
        }
        /* processo padre */
        
    }

    while (1)
    {
        int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
        int returnChild;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
        int pidChild;
        if ((pidChild = wait(&status)) < 0) {
            break;
        }
        
        if ((status & 0xFF) != 0)
        {
            printf("Il processo figlio è stato terminato in modo anomalo\n");
            exit(6);
        } else {
            returnChild = (status >> 8) & 0xFF;
        }

        if (returnChild == 255) {
            printf("Errore, il processo %d non e' riuscito ad aprire il file\n", pidChild);
        } else {
            printf("Il processo con PID %d ha ritornato %d\n", pidChild, returnChild);
        }
    }
    
    

    exit(0);
}