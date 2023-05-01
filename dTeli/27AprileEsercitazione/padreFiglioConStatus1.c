#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <fcntl.h>
#include <sys/wait.h>
#include <time.h>

int main(int argc, char** argv){
    int pid;	// memorizzo il valore di ritorno della funzione fork

    if ((pid = fork()) < 0)	/* Il processo padre crea un figlio */
    {	/* I  n questo caso la fork e' fallita */
        printf("Errore durante la fork\n");
        exit(1);
    }
    
    if (pid == 0)
    {	/*processo figlio */
        srand(time(NULL));
        int num = rand()%100;
        printf("%i \n",num);
        exit(num);
    }
    /* processo padre */
    
    
    
    int ritorno;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
    int numeroRitornato;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
    wait(&ritorno);
    if ((ritorno & 0xFF) != 0)
    {
        printf("Il processo figlio è stato terminato in modo anomalo\n");
        exit(1);
    } else {
        numeroRitornato = (ritorno >> 8) & 0xFF;
    }

    
    if (wait(&ritorno)<0)
    {
        printf("Il processo figlio è stato terminato in modo anomalo\n");
        exit(1);
    }
    
    printf("stampo pid %i, valore di ritorno %i\n",pid,numeroRitornato);    

    exit(0);
}