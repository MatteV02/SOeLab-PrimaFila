#include <stdio.h>		// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>		// Includo la libreria per la funzione exit
#include <unistd.h>		// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe,
#include <fcntl.h>		// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
//definisco il tipo pipe_t
typedef int pipe_t[2];

int main(int argc, char** argv) {
	//creo il file "Teli"
	char ok;
	int fcreato = 0; // variabile che conterra il file descriptor del file "Teli" */
	if((fcreato = creat("Teli",0644)) < 0){		/* ERRORE se non si riesce a creare il file */
		printf("Errore in creazione del file %s dato che fcreato = %d\n", "Teli", fcreato);
		exit(-1);
	}
	close(fcreato);
	pipe_t piped;
	
	if (pipe(piped) < 0 )
	{	printf("Errore creazione pipe\n");
		exit(-1);
	}
	int pidFiglio;	// memorizza il valore di ritorno della funzione fork
	
	if ((pidFiglio = fork()) < 0)	/* Il processo padre crea un figlio */
	{	/* In questo caso la fork e' fallita */
		printf("Errore durante la fork\n");
		exit(-1);
	}
	
	if (pidFiglio == 0)
	{	/* processo figlio */
		close(piped[1]);
		read(piped[0],&ok,1);
		exit(0);
	}
	/* processo padre */
	close(piped[0]);

	int pid;
	int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
	int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
	if ((pid = wait(&status)) < 0) {
		printf("Non e' stato creato nessun processo figlio\n");
		exit(-1);
	}
	
	if ((status & 0xFF) != 0)
	{
		printf("Il processo figlio con PID %d è stato terminato in modo anomalo\n", pidFiglio);
	} else {
		ritorno = (status >> 8) & 0xFF;
		printf("Il figlio di PID %d ha ritornato %d\n",pidFiglio,ritorno);
	}
	
	exit(0);
}
