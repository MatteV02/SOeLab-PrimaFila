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
		exit(-1);
	}
	int N=argc-1;
	//apro N pipes
	pipe_t* piped = (pipe_t*)malloc(sizeof(pipe_t) * (N));
	if (piped == NULL) {
		printf("Errore nell'allocazione della memoria\n");
		exit(-1);
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
		{	/* processo figlio */
			//chiudo tutte le pipe in lettura e scrittura tranne la i-esima pipe, aperta in scrittura
			for(int k = 0; k < N; k++){
				close(piped[k][0]);
				if (k != i) {
					close(piped[k][1]);
				}
			}
			//apro il file argv[i+1]
			int fd = 0; // variabile che conterra' il file descriptor del file fd
			if((fd = open(argv[i+1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire il file */
				printf("Errore in apertura file %s dato che fd = %d\n", argv[i+1], fd);
				exit(-1);
			}
			char buffer[250];	// carattere usato per leggere il contenuto del file
			int e=0;
			int prec=0;
			while (read(fd, &buffer[e], sizeof(char)) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
			{
				if(buffer[e]=='\n'){
					buffer[e]=0;
					prec=e;
					e=0;
				}
				else{
					e++;
				}
			}

			write(piped[i][1],&buffer,prec);
			exit(prec);
		}
		

	}
	/* processo padre */
	for(int i=0;i<N;i++){
		close(piped[i][1]);
	}
	int *e=malloc(sizeof(int)*N);
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
			printf("Il figlio di PID %d ha ritornato %d\n",pidFiglio,ritorno);
			e[i]=ritorno;

		}

		// controlla i pid e usa
	}
	//creo il file "UltimeLinee"
	int fd = 0; // variabile che conterra il file descriptor del file "UltimeLinee" */
	if((fd = creat("UltimeLinee", 0644)) < 0){		/* ERRORE se non si riesce a creare il file */
		printf("Errore in creazione del file %s dato che fd = %d\n", "UltimeLinee", fd);
		exit(-1);
	}	
	char buffer[250];
	for(int i=0;i<N;i++){
		read(piped[i][0],&buffer,e[i]);
		write(fd,&buffer,e[i]);
		
	}	
	exit(0);
}