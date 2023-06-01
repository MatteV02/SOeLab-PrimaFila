#include <stdio.h>		// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>		// Includo la libreria per la funzione exit
#include <unistd.h>		// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe,
#include <fcntl.h>		// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <ctype.h>
#include <string.h>
//definisco il tipo pipe_t
typedef int pipe_t[2];
struct dato{
	//indice
	int c1;
	//numero occorenze
	long int c2;
};

int main(int argc, char** argv) {

	int N=argc-2;
	char Cx;	// Memorizzo il singolo carattere nella variabile Cx
	struct dato* arr=0;
	int* pids=malloc(sizeof(int)*N);
	pipe_t* piped = (pipe_t*)malloc(sizeof(pipe_t) * (N+1));
	
	if (argc < 2 + 1) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 2*/
	{
		printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
		exit(-1);
	}
	
	//controlla che la stringa argv[argc-1] contenga un singolo carattere
	if (strlen(argv[argc-1]) != 1) {
		printf("Errore, la stringa %s non e' un singolo carattere\n", argv[argc-1]);
		exit(-1);
	}
	if(!islower(argv[argc-1][0])){
		printf("Errore, la stringa %s non e' in minuscolo\n", argv[argc-1]);
		exit(-1);
	}
	Cx = argv[argc-1][0];
	 
	//apro N+1 pipes
	if (piped == NULL) {
		printf("Errore nell'allocazione della memoria\n");
		exit(-1);
	}
	
	for(int i = 0; i < N+1; i++){
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
		pids[i]=pid;
		
		if (pid == 0)
		{	/* processo figlio */
			//chiudo tutte le pipe in lettura e scrittura tranne la i-esima pipe, aperta in scrittura
			for(int k = 0; k < N+1; k++){
		
				if(k!=i+1){
					close(piped[k][1]);
				}
				if (k==0 || k != i) {
					close(piped[k][0]);
				}
			}
			arr = malloc(sizeof(struct dato)*(i+1));
			//apro il file argv[i+1]
			int fd = 0; // variabile che conterra' il file descriptor del file argv[i+1]
			if((fd = open(argv[i+1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire il file */
				printf("Errore in apertura file %s dato che fd = %d\n", argv[i+1], fd);
				exit(-1);
			}
			char buffer;	// carattere usato per leggere il contenuto del file
			arr[i].c1=i;
			arr[i].c2=0;
			while (read(fd, &buffer, sizeof(char)) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
			{
				if(buffer==Cx){
					arr[i].c2++;
				}
			}
			if(i!=0){
			 	read(piped[i][0],arr,i*sizeof(struct dato));
			}
			for(int i=0;i<N;i++){
				printf("ha trovato %li al indice %i\n",arr[i].c2,arr[i].c1);
			}
			write(piped[i+1][1],arr,sizeof(struct dato)*(i+1));
			exit(i);
		}
	}
	/* processo padre */
	//chiudo tutte le pipe in lettura e scrittura tranne la N-esima pipe, aperta in lettura
	for(int k = 0; k < N+1; k++){
		close(piped[k][1]);
		if (k != N) {
			close(piped[k][0]);
		}
	}
	arr = malloc(sizeof(struct dato)*N);
	read(piped[N][0],arr,sizeof(struct dato)*N);
	for(int i=0;i<N;i++){
		printf("il figlio di pid %i ha trovato %li al indice %i\n",pids[arr[i].c1],arr[i].c2,arr[i].c1);
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
			printf("Il figlio di PID %d ha ritornato %d\n",pidFiglio,ritorno);
		}
	}

	exit(0);
}