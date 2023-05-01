/* FILE: provaWait.c */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main ()
{
   int pid;
	if ((pid = fork()) < 0)
	{ 	/* fork fallita */
		printf("Errore in fork\n");
		exit(1);
	}

	if (pid == 0)
	{ 	/* figlio */
		printf("Esecuzione del figlio\n");
        int valore=0;
        scanf("%i",&valore);
        if(valore<=255){
           if(valore<0){
            printf("valore che viene troncato\n");
           }
           else{
            printf("valore che non viene troncato\n");
           } 
           exit(valore);
        }
        exit(0);
			/* si torna un valore che si suppone possa essere derivante dall'esecuzione di un compito assegnato al figlio */
	}

	/* padre */
	printf("Generato figlio con PID = %d\n", pid);
	/* il padre aspetta il figlio disinteressandosi del valore della exit del figlio */
    int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
    int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
    wait(&status);
    
    if ((status & 0xFF) != 0)
    {
        printf("Il processo figlio è stato terminato in modo anomalo\n");
        exit(2);
    } else {
        ritorno = (status >> 8) & 0xFF;
    }
    
    printf("Numero= %d\n", ritorno);
	exit(0);

}
