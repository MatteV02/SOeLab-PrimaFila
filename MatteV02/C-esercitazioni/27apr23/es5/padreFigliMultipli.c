#include <stdio.h>    // Includo la libreria per la funzione printf
#include <stdlib.h>   // Includo la libreria per la funzione exit
#include <unistd.h>   // Includo la libreria per la funzione close e fork
#include <fcntl.h>    // Includo la libreria per la funzione open e le relative macro
#include <sys/wait.h> // Includo la libreria per la funzione wait

int main(int argc, char **argv)
{
    if (argc != 2) /* controllo sul numero di parametri: devono essere in numero uguale a 1*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }

    int N; /*Inizializzo varibile che conterra' argv[1]*/
    // controllo utlizzando la funzione atoi se il argv[1] e un numero strettamente positivo
    if ((N = atoi(argv[1])) <= 0 || N >= 255)
    {
        printf("il parametro %s non un numero positivo o è maggiore di 255", argv[1]);
        exit(2);
    }

    printf("Il PID del padre e' %d\n", getpid());

    printf("Il numero passato e' %d\n", N);

    for (int i = 0; i < N; i++)
    {
        int pidChild; // memorizzo il valore di ritorno della funzione fork

        if ((pidChild = fork()) < 0) /* Il processo padre crea un figlio */
        {                            /* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(3);
        }

        if (pidChild == 0)
        { /* processo figlio */
            printf("Il processo di ordine %d ha PID %d\n", i, getpid());
            exit(i);
        }
        /* processo padre */
    }

    while (1)
    {
        int status;       // La variabile usata per memorizzare quanto ritornato dalla primitiva wait
        int ritornoChild; // La variabile usata per memorizzare il valore di ritorno del processo figlio
        int pidChild;
        if ((pidChild = wait(&status)) < 0)
        {
            break;
        }

        if ((status & 0xFF) != 0)
        {
            printf("Il processo figlio è stato terminato in modo anomalo\n");
            exit(4);
        }
        else
        {
            ritornoChild = (status >> 8) & 0xFF;
        }

        printf("Terminato il processo con PID %d che ha ritornato %d\n", pidChild, ritornoChild);
    }

    exit(0);
}