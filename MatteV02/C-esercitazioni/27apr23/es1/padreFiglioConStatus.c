#include <stdio.h>	// Includo la libreria per la funzione printf
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close e fork
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <time.h>


int main(int argc, char** argv) {
    int myPID = getpid();

    printf("The PID of this process is %d\n", myPID);

    int childPID;	// memorizzo il valore di ritorno della funzione fork
    
    if ((childPID = fork()) < 0)	/* Il processo padre crea un figlio */
    {	/* In questo caso la fork e' fallita */
        printf("Errore durante la fork\n");
        exit(1);
    }
    
    if (childPID == 0)
    {	/* processo figlio */
        printf("The PID of this process is %d\n", getpid());
        srand(time(NULL));

        int randomNumber = rand() % 100;

        printf("Calcolo un numero random: %d\n", randomNumber);

        exit(randomNumber);
    }
    /* processo padre */
    
    int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
    int ritornoChild;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
    wait(&status);
    
    if ((status & 0xFF) != 0)
    {
        printf("Il processo figlio è stato terminato in modo anomalo\n");
        exit(2);
    } else {
        ritornoChild = (status >> 8) & 0xFF;
    }
    
    printf("Il valore di ritorno della funzionecon PID %d child e' %d\n", childPID, ritornoChild);

    exit(0);
}