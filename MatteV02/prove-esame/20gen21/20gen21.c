#include <stdio.h>		// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>		// Includo la libreria per la funzione exit
#include <unistd.h>		// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe,
#include <fcntl.h>		// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait


//definisco il tipo pipe_t
typedef int pipe_t[2];

int main(int argc, char** argv) {
	if (argc < 2 + 1) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 2*/
	{
		printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
		exit(1);
	}

	int Q = argc - 1;

	//apro Q pipes
	pipe_t* piped = (pipe_t*)malloc(sizeof(pipe_t) * (Q));
	if (piped == NULL) {
		printf("Errore nell'allocazione della memoria\n");
		exit(2);
	}
	
	for(int i = 0; i < Q; i++){
		if(pipe(piped[i])<0){
			printf("Errore nella creazione della pipe numero:%d\n",i);
			exit(2);
		}
	}

	char continua = 'c';

	for (int q = 0; q < Q; q++) {
		int pidFiglio;	// memorizza il valore di ritorno della funzione fork
		
		if ((pidFiglio = fork()) < 0)	/* Il processo padre crea un figlio */
		{	/* In questo caso la fork e' fallita */
			printf("Errore durante la fork\n");
			exit(3);
		}
		
		if (pidFiglio == 0)
		{	/* processo figlio */
			//chiudo tutte le pipe in lettura e scrittura tranne la q-esima pipe aperta in lettura e la (k + q)%Q-esima pipe aperta in scrittura
			for(int k = 0; k < Q; k++){
				if (k != q) {
					close(piped[k][0]);
					close(piped[(k + 1) % Q][1]);
				}
			}

			//apro il file argv[q + 1]
			int fd = 0; // variabile che conterra' il file descriptor del file argv[q + 1]
			if((fd = open(argv[q + 1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire il file */
				printf("Errore in apertura file %s dato che fd = %d\n", argv[q + 1], fd);
				exit(4);
			}

			char ch;	// carattere usato per leggere il contenuto del file
			int nLetti = 0;
			int k = 0;
			
			while (read(fd, &ch, sizeof(char)) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
			{
				if (q + k * Q == nLetti) {
					read(piped[q][0], &continua, sizeof(continua));
					printf("Figlio con indice %d e pid %d ha letto il carattere %c\n", q, getpid(), ch);
					write(piped[(q + 1) % Q][1], &continua, sizeof(continua));
					k++;
				}
				nLetti++;
			}

			exit(q);
		}
		/* processo padre */
		
	}

	//chiudo tutte le pipe in lettura e scrittura tranne la 0-esima pipe, aperta in scrittura
	for(int k = 0; k < Q; k++){
		close(piped[k][0]);
		if (k != 0) {
			close(piped[k][1]);
		}
	}

	write(piped[0][1], &continua, sizeof(continua));

	for (int q = 0; q < Q; q++) {
		int pidFiglio;
		int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
		int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
		if ((pidFiglio = wait(&status)) < 0) {
			printf("Non e' stato creato nessun processo figlio\n");
			exit(4);
		}
		
		if ((status & 0xFF) != 0)
		{
			printf("Il processo figlio con pid %d è stato terminato in modo anomalo\n", pidFiglio);
		} else {
			ritorno = (status >> 8) & 0xFF;
			printf("Il figlio di PID %d ha ritornato %d\n",pidFiglio,ritorno);
		}
	}

	exit(0);
}