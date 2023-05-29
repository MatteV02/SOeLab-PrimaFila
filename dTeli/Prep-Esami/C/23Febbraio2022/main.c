#include <stdio.h>	// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe, dup
#include <fcntl.h>	// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <ctype.h>
//definisco il tipo pipe_t
typedef int pipe_t[2];

int main(int argc, char** argv) {

	char car;		// carattere letto dal file assegnato al figlio
	int occorrenze=0;	// numero di numeri letti da un figlio
	long int somma=0; //totale trovato dai figli
	int N=argc-1; // totale figli
	int pid;	// memorizzo il valore di ritorno della funzione fork
	int fd = 0; /*variabile che conterra il file descriptor del file che stiamo per aprire */

	if (argc < 2 + 1) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 2*/
	{
		printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
		exit(-1);
	}
	
	//apro N pipes
	pipe_t* piped = (pipe_t*)malloc(sizeof(pipe_t) * (N));
	if (piped == NULL) {
		printf("Errore nell'allocazione della memoria\n");
		exit(-1);
	}
	//ciclo apertura N pipes
	for(int i = 0;i < N;i++){
		if(pipe(piped[i])<0){
			printf("Errore nella creazione della pipe numero:%i\n",i);
			exit(-1);
		}
	}
	for(int n=0;n<N;n++){	
		if ((pid = fork()) < 0)	/* Il processo padre crea un figlio */
		{	/* In questo caso la fork e' fallita */
			printf("Errore durante la fork\n");
			exit(-1);
		}
		
		if (pid == 0)
		{	/* processo figlio */
			//chiudo tutte le pipe in lettura e scrittura tranne la n-esima pipe, aperta in scrittura
			for(int k = 0; k < N; k++){
				close(piped[k][0]);
				if (k != n) {
					close(piped[k][1]);
				}
			}
			//controllo se il file e' accedibile
			if((fd = open(argv[n+1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
				printf("Errore in apertura file %s dato che fd = %d\n", argv[n+1], fd);
				exit(-1);
			}
			while (read(fd, &car, 1) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
			{
				//controllo se il carattere letto e un numero
				if(isdigit(car))
				{
					//converto il carattere e lo sommo	
					somma+=car-'0';	
					occorrenze++;
				}	
			}
			//scrivo la somma trovata dopo aver completato la lettura del file associato
			write(piped[n][1],&somma,sizeof(somma));
			exit(occorrenze);
		}		
	}
	/* processo padre */
	//chiudo tutte le pipe in scritture che non utilizzamo nel padre
	for(int i=0;i<N;i++){
		close(piped[i][1]);
	}
	//ciclo che riceve da tutti i figli la loro somma e le stampa in ordine F1->FN-1
	for(int i=0;i<N;i++){
		read(piped[i][0],&somma,sizeof(somma));
		printf("il totale del file %s indice %i e %li\n",argv[i+1],i,somma);
	}
	
	for(int i=0;i<N;i++){
		int pidFiglio;
		int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
		int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
		if ((pidFiglio = wait(&status)) < 0) {
			printf("Non e' stato creato nessun processo figlio\n");
			exit(-1);
		}
		
		if ((status & 0xFF) != 0)
		{
			printf("Il processo figlio è stato terminato in modo anomalo\n");
		} else {
			ritorno = (status >> 8) & 0xFF;
			printf("il figlio pid=%i ed ha ritornato=%i\n",pidFiglio,ritorno);
		}
	}
	exit(0);
}