#include <stdio.h>	// Includo la libreria per la funzione printf
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close e fork
#include <fcntl.h>	// Includo la libreria per la funzione open e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <time.h>
#include <string.h>


int main(int argc, char** argv) {
    if (argc < 4) /* controllo sul numero di parametri: devono essere in numero maggiore a 3*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }
    int* fds=malloc(sizeof(int)*argc-2);
    for(int i=1;i<argc-1;i++){
        fds[i-1]=open(argv[i],O_RDONLY);
    }
    if (strlen(argv[argc-1]) != 1) {	// RICORDATI #include <string.h>
        printf("Errore, la stringa %s non e' un singolo carattere\n", argv[argc-1]);
        exit(3);
    }
    char Cx = argv[argc-1][0];

        for(int i=0;i<argc-2;i++){
            int pid;	// memorizzo il valore di ritorno della funzione fork
            
            if ((pid = fork()) < 0)	/* Il processo padre crea un figlio */
            {	/* In questo caso la fork e' fallita */
                printf("Errore durante la fork\n");
                exit(3);
            }
            
            if (pid == 0)
            {	/* processo figlio */
                char c;
                int conta=0;
                while (read(fds[i], &c, 1) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
                {
                    if(c==Cx){
                        conta++;
                    }
                }
                exit(conta);
            }
            /* processo padre */
            
        }
        int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
        int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
        for(int i=0;i<argc-2;i++){
        wait(&status);
        
        if ((status & 0xFF) != 0)
        {
            printf("Il processo figlio è stato terminato in modo anomalo\n");
            exit(4);
        } else {
            ritorno = (status >> 8) & 0xFF;
            printf("il file %s ha ritornato %d\n",argv[i+1],ritorno);
        }
        }

    exit(0);
}