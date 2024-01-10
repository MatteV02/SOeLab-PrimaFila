#include <stdio.h>		// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>		// Includo la libreria per la funzione exit
#include <unistd.h>		// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe,
#include <fcntl.h>		// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <ctype.h>
//definisco il tipo pipe_t
typedef int pipe_t[2];

int main(int argc, char** argv) {
	if (argc < 2 + 1) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 2*/
	{
		printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
		exit(-1);
	}
	int N=argc-1;
	//apro N pipes
	pipe_t* piped = (pipe_t*)malloc(sizeof(pipe_t) * (N));
	if (piped == NULL) {
		printf("Errore nell'allocazione della memoria\n");
		exit(-1);
	}
	//apro N pipes
	pipe_t* pipedN = (pipe_t*)malloc(sizeof(pipe_t) * (N));
	if (pipedN == NULL) {
		printf("Errore nell'allocazione della memoria\n");
		exit(-1);
	}
	
	for(int i = 0; i < N; i++){
		if(pipe(pipedN[i])<0){
			printf("Errore nella creazione della pipe numero:%d\n",i);
			exit(-1);
		}
	}
	
	for(int i = 0; i < N; i++){
		if(pipe(piped[i])<0){
			printf("Errore nella creazione della pipe numero:%d\n",i);
			exit(-1);
		}
	}	

	for(int i=0;i<N;i++){
		int pid;	// memorizza il valore di ritorno della funzione fork
		
		if ((pid = fork()) < 0)	/* Il processo padre crea un figlio */
		{	/* In questo caso la fork e' fallita */
			printf("Errore durante la fork\n");
			exit(-1);
		}
		
		if (pid == 0)
		{	
			int pidF;	// memorizza il valore di ritorno della funzione fork
			
			if ((pidF = fork()) < 0)	/* Il processo padre crea un figlio */
			{	/* In questo caso la fork e' fallita */
				printf("Errore durante la fork\n");
				exit(-1);
			}
			
			if (pidF == 0)
			{	/* processo nipote */
				for(int k = 0; k < N; k++){
					close(pipedN[k][0]);
					close(piped[k][0]);
					close(piped[k][1]);
					if (k != i) {
						close(pipedN[k][1]);
					}
				}
				//apro il file argv[i+1]
				int fd = 0; // variabile che conterra' il file descriptor del file argv[i+1]
				if((fd = open(argv[i+1], O_RDWR)) < 0){		/* ERRORE se non si riesce ad aprire il file */
					printf("Errore in apertura file %s dato che fd = %d\n", argv[i+1], fd);
					exit(-1);
				}
				char buffer=0;	// carattere usato per leggere il contenuto del file
				long int occ=0;
				while (read(fd, &buffer, sizeof(char)) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
				{
					if(isdigit(buffer)){
						buffer=' ';
						occ++;
						lseek(fd,-1L,SEEK_CUR);
						write(fd,&buffer,sizeof(buffer));
					}
				}
				write(pipedN[i][1],&occ,sizeof(occ));
				exit(occ%256);
			}
			/* processo figlio */
			//chiudo tutte le pipe in lettura e scrittura tranne la i-esima pipe, aperta in scrittura
			for(int k = 0; k < N; k++){
				close(piped[k][0]);
				close(pipedN[k][0]);
				close(pipedN[k][1]);
				if (k != i) {
					close(piped[k][1]);
				}
			}
			//apro il file argv[i+1]
			int fd = 0; // variabile che conterra' il file descriptor del file argv[i+1]
			if((fd = open(argv[i+1], O_RDWR)) < 0){		/* ERRORE se non si riesce ad aprire il file */
				printf("Errore in apertura file %s dato che fd = %d\n", argv[i+1], fd);
				exit(-1);
			}
			char buffer;	// carattere usato per leggere il contenuto del file
			long int occ=0;
			while (read(fd, &buffer, sizeof(char)) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
			{
				if(islower(buffer)){
					occ++;
					buffer=toupper(buffer);
					lseek(fd,-1,SEEK_CUR);
					write(fd,&buffer,sizeof(buffer));
				}
			}
			write(piped[i][1],&occ,sizeof(occ));
			int pidNipote;
			int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
			int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
			if ((pidNipote = wait(&status)) < 0) {
				printf("Non e' stato creato nessun processo figlio\n");
				exit(-1);
			}
			
			if ((status & 0xFF) != 0)
			{
				printf("Il processo figlio con PID %d è stato terminato in modo anomalo\n", pidNipote);
			} else {
				ritorno = (status >> 8) & 0xFF;
				printf("Il Nipote di PID %d ha ritornato %d\n",pidNipote,ritorno);
			}
			exit(occ%256);
		}
	}
	/* processo padre */
	for(int i=0;i<N;i++){
		close(piped[i][1]);
		close(pipedN[i][1]);
	}
	long int occ=0;
	for(int i=0;i<N;i++){
		read(piped[i][0],&occ,sizeof(occ));
		printf("il figlio di indice %i legge il file %s ha sostituito %li numeri\n",i,argv[i+1],occ);
		read(pipedN[i][0],&occ,sizeof(occ));
		printf("il nipote di indice %i legge il file %s ha sostituito %li numeri\n",i,argv[i+1],occ);

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
			printf("Il processo figlio con PID %d è stato terminato in modo anomalo\n", pidFiglio);
		} else {
			ritorno = (status >> 8) & 0xFF;
			printf("Il figlio di PID %d ha ritornato %d\n",pidFiglio,ritorno);	
		}
	}
	exit(0);
}