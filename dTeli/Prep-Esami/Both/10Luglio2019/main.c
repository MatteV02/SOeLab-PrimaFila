#include <stdio.h>		// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>		// Includo la libreria per la funzione exit
#include <unistd.h>		// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe,
#include <fcntl.h>		// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <string.h>
//definisco il tipo pipe_t
typedef int pipe_t[2];

int main(int argc, char** argv) {
	
	if (argc < 3 + 1) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 3*/
	{
		printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
		exit(-1);
	}
	//controlla che la stringa argv[argc-1] contenga un singolo carattere
	if (strlen(argv[argc-1]) != 1) {
		printf("Errore, la stringa %s non e' un singolo carattere\n", argv[argc-1]);
		exit(-1);
	}
	char Cz = argv[argc-1][0];	// Memorizzo il singolo carattere nella variabile Cz
	int N=argc-2;
	
	//apro N*2 pipes
	pipe_t* piped = (pipe_t*)malloc(sizeof(pipe_t) * (N*2));
	if (piped == NULL) {
		printf("Errore nell'allocazione della memoria\n");
		exit(-1);
	}
	
	for(int i = 0; i < N*2; i++){
		if(pipe(piped[i])<0){
			printf("Errore nella creazione della pipe numero:%d\n",i);
			exit(-1);
		}
	}

	for(int i=0; i<N*2;i++){
		int pied;	// memorizza il valore di ritorno della funzione fork
		
		if ((pied = fork()) < 0)	/* Il processo padre crea un figlio */
		{	/* In questo caso la fork e' fallita */
			printf("Errore durante la fork\n");
			exit(-1);
		}
		
		if (pied == 0)
		{	/* processo figlio */
			//chiudo tutte le pipe in lettura e scrittura tranne la i-esima pipe, aperta in scrittura
			char buffer;	// carattere usato per leggere il contenuto del file
			long int contatore=0;
			int occorrenze=0;
			int scrittura=2*N-1-i;	
			for(int k = 0; k < N*2; k++){
				if (k != i) {
					close(piped[k][0]);
				}
				if(k != scrittura){
					close(piped[k][1]);
				}
			}
			//apro il file argv[2*N-i] o argv[i+1]
			int fd = 0; // variabile che conterra' il file descriptor del file argv[2*N-i] o argv[i+1]
			if(i>=N){
				if((fd = open(argv[2*N-i], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire il file */
					printf("Errore in apertura file %s dato che fd = %d\n", argv[2*N-i], fd);
					exit(-1);
				}
			read(piped[i][0],&contatore,sizeof(contatore));
			lseek(fd,++contatore,SEEK_SET);		
			while (read(fd, &buffer, sizeof(char)) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
			{
				if(Cz==buffer){
					occorrenze++;
					write(piped[scrittura][1],&contatore,sizeof(contatore));
					read(piped[i][0],&contatore,sizeof(contatore));
					lseek(fd,++contatore,SEEK_SET);
				}
				else{
					contatore++;
				}
			}
			}
			else{
				if((fd = open(argv[i+1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire il file */
					printf("Errore in apertura file %s dato che fd = %d\n", argv[i+1], fd);
					exit(-1);
				}
			while (read(fd, &buffer, sizeof(char)) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
			{
				if(Cz==buffer){
					occorrenze++;
					write(piped[scrittura][1],&contatore,sizeof(contatore));
					read(piped[i][0],&contatore,sizeof(contatore));
					lseek(fd,++contatore,SEEK_SET);
				}
				else{
					contatore++;
				}
			}
			}


			//close(piped[scrittura][1]);
			exit(occorrenze);
		}
	}
	/* processo padre */
	//chiudo tutte le pipe in lettura e scrittura tranne la i-esima pipe, aperta in lettura
	for(int k = 0; k < N*2; k++){
		close(piped[k][1]);
		close(piped[k][0]);
	
	}
	for(int i=0;i<2*N;i++){
		int pidFiglio;
		int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
		int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
		if ((pidFiglio = wait(&status)) < 0) {
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
	}

	exit(0);
}