#include <stdio.h>		// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>		// Includo la libreria per la funzione exit
#include <unistd.h>		// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe,
#include <fcntl.h>		// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
//definisco il tipo pipe_t
typedef int pipe_t[2];
struct dato{
	//lungezza linee
	long int c1;
	//indice ordine
	int c2;
};

int main(int argc, char** argv) {
	
	if (argc < 1 + 1) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 1*/
	{
		printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
		exit(-1);
	}
	int M=argc-1;
	//apro M+1 pipes
	pipe_t* piped = (pipe_t*)malloc(sizeof(pipe_t) * (M+1));
	if (piped == NULL) {
		printf("Errore nell'allocazione della memoria\n");
		exit(-1);
	}
	
	for(int i = 0; i < M+1; i++){
		if(pipe(piped[i])<0){
			printf("Errore nella creazione della pipe numero:%d\n",i);
			exit(-1);
		}
	}
	struct dato* arr=malloc(M*sizeof(struct dato));
	int* pid=malloc(M*sizeof(int));	// memorizza il valore di ritorno della funzione fork

	for(int i=0;i<M;i++){
				
		if ((pid[i] = fork()) < 0)	/* Il processo padre crea un figlio */
		{	/* In questo caso la fork e' fallita */
			printf("Errore durante la fork\n");
			exit(-1);
		}
		
		if (pid[i] == 0)
		{	/* processo figlio */
			//chiudo tutte le pipe in lettura e scrittura tranne la i-esima pipe, aperta in scrittura
			for(int k = 0; k < M+1; k++){
				
				if (k != i || k == 0 ) {
					close(piped[k][0]);
				}
				if(k!=i+1){
					close(piped[k][1]);
				}

			}
			//apro il file argv[i+1]
			int fd = 0; // variabile che conterra' il file descriptor del file argv[i+1]
			if((fd = open(argv[i+1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire il file */
				printf("Errore in apertura file %s dato che fd = %d\n", argv[i+1], fd);
				exit(-1);
			}

			if(i!=0){
				read(piped[i][0],arr,sizeof(struct dato)*M);
			}
			char buffer;	// carattere usato per leggere il contenuto del file
			arr[i].c1=0;
			arr[i].c2=i;
			while (read(fd, &buffer, sizeof(char)) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
			{
				if(buffer=='\n'){
					arr[i].c1++;
				}
			} 
			write(piped[i+1][1],arr,sizeof(struct dato)*M);
			exit(i);
		}
	}
	/* processo padre */
	for(int i=0;i<M+1;i++){
		close(piped[i][1]);
		if(i!=M){
			close(piped[i][0]);
		}
	}

	read(piped[M][0],arr,sizeof(struct dato)*M);
	for(int i=0;i<M;i++){
		printf("il figlio=%i di indice %i ha trovato %li\n",pid[i],arr[i].c2,arr[i].c1);
	}

	for(int i=0;i<M;i++){
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