#include <stdio.h>	// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe, dup
#include <fcntl.h>	// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
//definisco il tipo pipe_t
typedef int pipe_t[2];

int main(int argc, char** argv) {
	
	int N=argc-1;
	pipe_t* pipedS = (pipe_t*)malloc(sizeof(pipe_t) * (N));
	pipe_t* pipedD = (pipe_t*)malloc(sizeof(pipe_t) * (N));
	char* terminati=malloc(N*1);
	char go=0;
	char terminator=0;
	char buffer;
	char linea[250];
	int lunglinea=0;
	int scritture=0;

	if (argc < 2 + 1) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 2*/
	{
		printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
		exit(-1);
	}
	//apro N pipes
	if (pipedD == NULL) {
		printf("Errore nell'allocazione della memoria\n");
		exit(-1);
	}
	
	for(int i = 0;i < N;i++){
		if(pipe(pipedD[i])<0){
			printf("Errore nella creazione della pipe numero:%i\n",i);
			exit(-1);
		}
	}
	//apro N pipes
	if (pipedS == NULL) {
		printf("Errore nell'allocazione della memoria\n");
		exit(-1);
	}
	
	for(int i = 0;i < N;i++){
		if(pipe(pipedS[i])<0){
			printf("Errore nella creazione della pipe numero:%i\n",i);
			exit(-1);
		}
	}

	for(int i=0;i<N;i++){
		int pid;	// memorizzo il valore di ritorno della funzione fork
		
		if ((pid = fork()) < 0)	/* Il processo padre crea un figlio */
		{	/* In questo caso la fork e' fallita */
			printf("Errore durante la fork\n");
			exit(-1);
		}
		
		if (pid == 0)
		{	/* processo figlio */
			//chiudo tutte le pipe in lettura e scrittura tranne la i-esima pipe, aperta in scrittura
			for(int k = 0; k < N; k++){
				close(pipedD[k][0]);
				close(pipedS[k][1]);
				if (k != i) {
					close(pipedD[k][1]);
					close(pipedS[k][0]);
				}
			}
			//controllo se il file e' accedibile
			int fd = 0; /*variabile che conterra il file descriptor del file che stiamo per aprire */
			if((fd = open(argv[i+1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
				printf("Errore in apertura file %s dato che fd = %d\n", argv[i+1], fd);
				exit(-1);
			}

			while (read(fd, &buffer, 1) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
			{
				//printf("DEBUG:FIGLIO %i LEGGE %c\n",i,buffer);
				if(buffer=='\n'){
					write(pipedD[i][1],&linea[0],sizeof(linea[0]));
					read(pipedS[i][0],&go,1);
					linea[lunglinea]=0;
					lunglinea=0;
				}
				else{
					linea[lunglinea]=buffer;
					lunglinea++;
				}
				if(go==1){
					printf("il figlio di indice %i e pid=%i ha il carattere minimo %c\n",i,getpid(),linea[0]);
					printf("la linea= %s\n\n",linea);
					scritture++;
					go=0;
				}
			}
			exit(scritture);
		}
		
		
	}
	/* processo padre */
	//chiudo tutte le pipe in lettura e scrittura
	for(int k = 0; k < N; k++){
		close(pipedD[k][1]);
		close(pipedS[k][0]);
	}

	char min;
	int indice;
	int nread;
	while(terminator==0){
		terminator=1;
		min=-1;
		for(int i=0;i<N;i++){
			if((nread=read(pipedD[i][0],&buffer,sizeof(buffer)))>0){
				terminator=0;
				if(min==-1 || min>buffer){
					min=buffer;
					indice=i;
				}
			}
			if(nread==0){
				terminati[i]=1;
			}
		}
		
		for(int i=0;i<N;i++){
			if(terminati[i]!=1){
				if(i==indice){
					go=1;
				}
				else{
					go=0;
				}
				write(pipedS[i][1],&go,sizeof(go));
			}
		}
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