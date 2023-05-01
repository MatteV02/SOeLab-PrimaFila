#include <stdio.h>	// Includo la libreria per la funzione printf
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close e fork
#include <fcntl.h>	// Includo la libreria per la funzione open e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <time.h>


int main(int argc, char** argv) {
    if (argc < 2) /* controllo sul numero di parametri: devono essere in numero uguale a 1*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }
    int N;		/*Inizializzo varibile che conterra argv[1]*/
    //controllo utlizzando la funzione atoi se il argv[1] e un numero positivo
    if((N=atoi(argv[1]))<=0 && N>155){
        printf("il parametro %s non un numero positivo",argv[1]);
        exit(2);
    }
    
    int *pidFigli=malloc(sizeof(int)*N);
    printf("il pid del padre=%i il numero=%i\n",getpid(),N);
        for(int i=0;i<N;i++){
            int pid;	// memorizzo il valore di ritorno della funzione fork
            
            if ((pid = fork()) < 0)	/* Il processo padre crea un figlio */
            {	/* In questo caso la fork e' fallita */
                printf("Errore durante la fork\n");
                exit(3);
            }
            
            if (pid == 0)
            {	/* processo figlio */
                srand(time(NULL));
                int casuale = rand()%(101+i);
                printf("il pid del figlio=%i il numero=%i\n",getpid(),i);
                exit(casuale);
            }
            pidFigli[i]=pid;
            /* processo padre */
            
        }
        int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
        int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
        for(int i=0;i<N;i++){
        wait(&status);
        
        if ((status & 0xFF) != 0)
        {
            printf("Il processo figlio è stato terminato in modo anomalo\n");
            exit(4);
        } else {
            ritorno = (status >> 8) & 0xFF;
            printf("Il figlio-%i con pid=%d ha ritornato %d\n",i+1,pidFigli[i], ritorno);
        }
        }

    exit(0);
}