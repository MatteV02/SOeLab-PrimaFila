#include <stdio.h>	// Includo la libreria per la funzione printf
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close e fork
#include <fcntl.h>	// Includo la libreria per la funzione open e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <string.h>
#include <errno.h>
#define PERM 0644
#define ever (;;)
int main(int argc, char** argv) {
    if (argc < 4) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 3*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }

    for(int i=1;i<argc;i++){
        int pidChil;	// memorizzo il valore di ritorno della funzione fork
        
        if ((pidChil = fork()) < 0)	/* Il processo padre crea un figlio */
        {	/* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(2);
        }
        if (pidChil == 0)
        {	/* processo figlio */
            
            char fileout[256] = "";
            strcat(fileout, argv[i]);
            strcat(fileout, ".sort");
            close(1);
            creat(fileout,PERM);
            int pidNipote;	// memorizzo il valore di ritorno della funzione fork
            if ((pidNipote = fork()) < 0)	/* Il processo padre crea un figlio */
            {	/* In questo caso la fork e' fallita */
                printf("Errore durante la fork\n");
                exit(2);
            }
            if (pidNipote == 0)
            {	/* processo figlio */
                //controllo se il file e' accedibile
                close(0);

                if((open(argv[i], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
                    fprintf(stderr,"Errore in apertura file %s \n", argv[i]);
                    exit(4);
                }
                execlp("sort","sort",(char *)0);
                exit(-2);
            }
            /* processo padre */
            int pidCug;
            int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
            int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
            if ((pidCug = wait(&status)) < 0) {
                printf("Non e' stato creato nessun processo figlio\n");
                exit(1);
            }
            
            if ((status & 0xFF) != 0)
            {
                printf("Il processo figlio è stato terminato in modo anomalo\n");
                exit(1);
            } else {
                ritorno = (status >> 8) & 0xFF;
                if(ritorno<0){
                exit(-1);
                }
            }
            exit(0);
        }
    }
        for (int i=0;i<argc-1;i++){
        /* processo padre */
        int pidChild;
        int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
        int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
        if ((pidChild = wait(&status)) < 0) {
            break;
        }
        
        if ((status & 0xFF) != 0)
        {
            printf("Il processo figlio è stato terminato in modo anomalo\n");
            exit(2);
        } else {
            ritorno = (status >> 8) & 0xFF;
        }
        printf("Il figlio con pid=%d ha ritornato %d (se 255 problemi!)\n", pidChild, ritorno);
    }
        
        
    exit(0);
}