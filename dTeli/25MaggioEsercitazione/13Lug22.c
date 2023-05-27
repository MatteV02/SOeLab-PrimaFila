#include <stdio.h>	// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe, dup
#include <fcntl.h>	// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <string.h>
//definisco il tipo pipe_t
typedef int pipe_t[2];

int main(int argc, char** argv) {
	if (argc < 2 + 1) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 2*/
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
	int Q=argc-3;
	char ok=0;
	//apro Q pipes
	pipe_t* piped = (pipe_t*)malloc(sizeof(pipe_t) * (Q+1));
	if (piped == NULL) {
		printf("Errore nell'allocazione della memoria\n");
		exit(-1);
	}
	
	for(int i = 0;i <= Q;i++){
		if(pipe(piped[i])<0){
			printf("Errore nella creazione della pipe numero:%i\n",i);
			exit(-1);
		}
	}
	for(int q=0;q<Q;q++){
		int pid;	// memorizzo il valore di ritorno della funzione fork
		
		if ((pid = fork()) < 0)	/* Il processo padre crea un figlio */
		{	/* In questo caso la fork e' fallita */
			printf("Errore durante la fork\n");
			exit(-1);
		}
		
		if (pid == 0)
		{	/* processo figlio */
			//chiudo tutte le pipe in lettura e scrittura tranne la q-esima pipe, aperta in scrittura
			for(int k = 0; k <= Q; k++){
				if (k != q) {
					close(piped[k][0]);   
				}
				if(k != q+1){
	
					close(piped[k][1]);
				}
			}
			//controlla che la stringa argv[i+2] sia un singolo carattere
			if (strlen(argv[q+3]) != 1) {	
				printf("Errore, la stringa %s non e' un singolo carattere\n", argv[q+2]);
				exit(-1);
			}
			char C = argv[q+3][0];
			//controllo se il file e' accedibile
			int fd = 0; /*variabile che conterra il file descriptor del file che stiamo per aprire */
			if((fd = open(argv[1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
				printf("Errore in apertura file %s dato che fd = %d\n", argv[1], fd);
				exit(-1);
			}
			char buffer;
			int occorenze=0;
			int nread=0;
			while(read(fd, &buffer, 1) > 0){

				if(buffer=='\n'){
					
					read(piped[q][0],&ok,sizeof(ok));
					printf("Figlio con indice %i e pid=%i ha letto %i caratteri %c nella linea corrente\n",q,getpid(),occorenze,C);
					nread=occorenze;
					occorenze=0;
					write(piped[q+1][1],&ok,sizeof(ok));
				}
				if(buffer==C){
					occorenze++;
				}   
			}
	
			close(piped[q][1]);
			exit(nread);
		}
	}
	/* processo padre */
		for(int k = 0; k <= Q; k++){
			if(k!=0){
				close(piped[k][1]);
			}
			if(k!=Q){
				close(piped[k][0]);
			}
	}
	for(int i=0;i<L;i++){
		printf("Linea %i del file %s\n",i,argv[1]);   
		write(piped[0][1],&ok,sizeof(ok));
		read(piped[Q][0],&ok,1);
	}
	close(piped[0][1]);
	close(piped[Q][0]);
	//chiudo tutte le pipe in lettura e scrittura tranne la i-esima pipe, aperta in scrittura

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
			printf("Il processo figlio è stato terminato in modo anomalo\n");
		} else {
			ritorno = (status >> 8) & 0xFF;
			printf("il figlio pid=%i ed ha ritornato=%i\n",pidFiglio,ritorno);  
		}
	}
	exit(0);
}