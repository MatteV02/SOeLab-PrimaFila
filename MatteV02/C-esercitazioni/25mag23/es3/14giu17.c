#include <stdio.h>	// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe,
#include <fcntl.h>	// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <stdbool.h>
#include <string.h>

//definisco il tipo pipe_t
typedef int pipe_t[2];

int main(int argc, char** argv) {
	if (argc < 2 + 1) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 2*/
	{
		printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
		exit(1);
	}

	//controlla che la stringa argv[argc - 1] sia un singolo carattere
	if (strlen(argv[argc - 1]) != 1) {
		printf("Errore, la stringa %s non e' un singolo carattere\n", argv[argc - 1]);
		exit(2);
	}
	char Cx = argv[argc - 1][0];

	int N = argc - 2;

	//apro N pipes
	pipe_t* pipedFP = (pipe_t*)malloc(sizeof(pipe_t) * (N));
	if (pipedFP == NULL) {
		printf("Errore nell'allocazione della memoria\n");
		exit(3);
	}
	
	for(int i = 0;i < N;i++){
		if(pipe(pipedFP[i])<0){
			printf("Errore nella creazione della pipe numero:%i\n",i);
			exit(3);
		}
	}

	//apro N pipes
	pipe_t* pipedPF = (pipe_t*)malloc(sizeof(pipe_t) * (N));
	if (pipedPF == NULL) {
		printf("Errore nell'allocazione della memoria\n");
		exit(4);
	}
	
	for(int i = 0;i < N;i++){
		if(pipe(pipedPF[i])<0){
			printf("Errore nella creazione della pipe numero:%i\n",i);
			exit(4);
		}
	}

	for (int i = 0; i < N; i++)
	{
		int pidFiglio;	// memorizzo il valore di ritorno della funzione fork
		
		if ((pidFiglio = fork()) < 0)	/* Il processo padre crea un figlio */
		{	/* In questo caso la fork e' fallita */
			printf("Errore durante la fork\n");
			exit(5);
		}
		
		if (pidFiglio == 0)
		{	/* processo figlio */
			//chiudo tutte le pipe in lettura e scrittura tranne la i-esima pipe, aperta in scrittura
			for(int k = 0; k < N; k++){
				close(pipedFP[k][0]);
				close(pipedPF[k][1]);
				if (k != i) {
					close(pipedFP[k][1]);
					close(pipedPF[k][0]);
				}
			}

			//controllo se il file e' accedibile
			int fd = 0; /*variabile che conterra il file descriptor del file che stiamo per aprire */
			if((fd = open(argv[i + 1], O_RDWR)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
				printf("Errore in apertura file %s dato che fd = %d\n", argv[i + 1], fd);
				exit(255);
			}

			char c;
			int nSostituzioni = 0;
			
			while (read(fd, &c, sizeof(char)) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
			{
				if (c == Cx) {
					long int pos = lseek(fd, 0, SEEK_CUR);
					write(pipedFP[i][1], &pos, sizeof(pos));

					read(pipedPF[i][0], &c, sizeof(c));
					printf("DEBUG:il figlio %d ha ricevuto il carattere %c dal padre\n", i, c);
					if (c != '\n') {
						lseek(fd, -1, SEEK_CUR);
						write(fd, &c, sizeof(c));
						nSostituzioni++;
					}
				}
			}

			exit(nSostituzioni);
		}
		/* processo padre */
		
	}

	for (int i = 0; i < N; i++)
	{
		close(pipedPF[i][0]);
		close(pipedFP[i][1]);
	}

	int i = 0;
	long int pos;
	char c;
	int nPipeChiuse = 0;
	bool *pipeChiusa = calloc(N, sizeof(*pipeChiusa));
	
	while (nPipeChiuse < N) {
		if (!pipeChiusa[i % N]) {
			if (read(pipedFP[i % N][0], &pos, sizeof(pos)) == 0) {
				pipeChiusa[i % N] = true;
				nPipeChiuse++;
			} else {
				printf("Trovato il carattere %c nel file \"%s\", vuoi sostituirlo? (a capo -> NO, altrimenti sostituisci col carattere digitato):\n", Cx, argv[i % N + 1]);
				read(0, &c, sizeof(c));
				write(pipedPF[i % N][1], &c, sizeof(c));
				printf("DEBUG:letto il carattere %i\n", (int)c);
				if (c != '\n') {
					printf("DEBUG:scarto caporiga\n");
					read(0, &c, sizeof(c));
					if (c != '\n') {
						printf("Errore, devi digitare un solo carattere!\n"); 
						exit(6);
					}
				}
			}
		}
		i++;
	}

	for (int i = 0; i < N; i++) {
		int pidFiglio;
		int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
		int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
		if ((pidFiglio = wait(&status)) < 0) {
			printf("Non e' stato creato nessun processo figlio\n");
			exit(7);
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