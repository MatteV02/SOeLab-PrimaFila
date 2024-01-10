#include <stdio.h>		// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>		// Includo la libreria per la funzione exit
#include <unistd.h>		// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe,
#include <fcntl.h>		// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <string.h>
//definisco il tipo pipe_t
typedef int pipe_t[2];
int main(int argc, char** argv) {
	if (argc < 4 + 1) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 4*/
	{
		printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
		exit(-1);
	}
	int L;		/*Dichiarazione varibile che conterra' argv[2]*/
	//controllo utlizzando la funzione atoi se il argv[2] e un numero strettamente positivo
	if((L = atoi(argv[2])) <= 0){
		printf("il parametro %s non un numero positivo\n",argv[2]);
		exit(-1);
	}
	char *C=malloc(1*argc-3);
	for(int i=3;i<argc;i++){
		printf("DEBUG:argv=%c\n",argv[i][0]);
		//controlla che la stringa argv[i] contenga un singolo carattere
		if (strlen(argv[i]) != 1) {	
			printf("Errore, la stringa %s non e' un singolo carattere\n", argv[i]);
			exit(-1);
		}
		C[i-3] = argv[i][0];	// Memorizzo il singolo carattere nella variaQ[i]
		printf("DEBUG:C=%c\n",C[i-3]);
	}
	int Q=argc-3;
	//apro N pipes
	pipe_t* piped = (pipe_t*)malloc(sizeof(pipe_t) * (Q+1));
	if (piped == NULL) {
		printf("Errore nell'allocazione della memoria\n");
		exit(-1);
	}
	
	for(int i = 0; i < Q+1; i++){
		if(pipe(piped[i])<0){
			printf("Errore nella creazione della pipe numero:%d\n",i);
			exit(-1);
		}
	}	
	char ok=0;

	for(int q=0;q<Q;q++){
		int pid;	// memorizza il valore di ritorno della funzione fork
		
		if ((pid = fork()) < 0)	/* Il processo padre crea un figlio */
		{	/* In questo caso la fork e' fallita */
			printf("Errore durante la fork\n");
			exit(-1);
		}
		
		if (pid == 0)
		{	/* processo figlio */
			//chiudo tutte le pipe in lettura e scrittura tranne la q-esima pipe, aperta in scrittura
			for(int k = 0; k < Q; k++){
				if(k != q+1){
					close(piped[k][1]);
				}
				if (k != q) {
					close(piped[k][0]);
				}
			}
			//apro il file argv[2]
			int fd = 0; // variabile che conterra' il file descriptor del file argv[2]
			if((fd = open(argv[1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire il file */
				printf("Errore in apertura file %s dato che fd = %d\n", argv[1], fd);
				exit(-1);
			}
			char buffer;	// carattere usato per leggere il contenuto del file
			int occ=0;
			while (read(fd, &buffer, sizeof(char)) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
			{
				if(buffer=='\n'){
					read(piped[q][0],&ok,sizeof(ok));
					printf("Figlio con indice %i e pid %i ha letto %i carattere %c nella linea corrente\n",q,getpid(),occ,C[q]);
					write(piped[q+1][1],&ok,sizeof(ok));
					occ=0;
				}
				if(buffer==C[q]){
					occ++;
				}
			}

			
			exit(0);
		}
		
	}
	/* processo padre */
	for(int k = 0; k < Q; k++){
	if(k != 0){
		close(piped[k][1]);
	}
	if (k != Q) {
		close(piped[k][0]);
	}
	}	

	for(int i=0;i<L;i++){
		printf("Linea %i del file %s\n",i,argv[1]);
		write(piped[0][1],&ok,1);
		read(piped[Q][0],&ok,1);
	}

	for(int i=0;i<Q;i++){
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