#include <stdio.h>	// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe, dup
#include <fcntl.h>	// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
//definisco il tipo pipe_t
typedef int pipe_t[2];

int main(int argc, char** argv) {
	int N=argc-2;
	//apro N pipes
	pipe_t* pipedF = (pipe_t*)malloc(sizeof(pipe_t) * (N));
	if (pipedF == NULL) {
		printf("Errore nell'allocazione della memoria\n");
		exit(-1);
	}
	
	for(int i = 0;i < N;i++){
		if(pipe(pipedF[i])<0){
			printf("Errore nella creazione della pipe numero:%i\n",i);
			exit(-1);
		}
	}

	//controllo se il file e' accedibile
	int fdw = 0; /*variabile che conterra il file descriptor del file che stiamo per aprire */
	if((fdw = open(argv[argc-1], O_RDWR,O_APPEND)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
		printf("Errore in apertura file %s dato che fd = %d\n", argv[argc-1], fdw);
		exit(-1);
	}

	for(int n=0;n<N;n++){
		int pid;	// memorizzo il valore di ritorno della funzione fork
		
		if ((pid = fork()) < 0)	/* Il processo padre crea un figlio */
		{	/* In questo caso la fork e' fallita */
			printf("Errore durante la fork\n");
			exit(-1);
		}
		
		if (pid == 0)
		{	/* processo figlio */
			//chiudo tutte le pipe in lettura e scrittura tranne la n-esima pipe, aperta in scrittura
			for(int k = 0; k < N; k++){
				close(pipedF[k][0]);
				if (k != n) {
					close(pipedF[k][1]);
				}
			}
			//controllo se il file e' accedibile
			int fd = 0; /*variabile che conterra il file descriptor del file che stiamo per aprire */
			if((fd = open(argv[n+1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
				printf("Errore in apertura file %s dato che fd = %d\n", argv[n+1], fd);
				exit(-1);
			}
			char chs[2];    // buffer che verra' letto da file
			int nread;              // variabile che memorizzera' il ritorno della read
			int ntot=0;
			while ((nread = read(fd, chs, 2)) > 0 )	/* ciclo di lettura fino a che nread e' maggiore di 0 */
			{
				printf("DEBUG:letto i car %c %c\n",chs[0],chs[1]);
				if(nread!=2){
					write(pipedF[n][1],&chs,1);
				}	
				else{
					write(pipedF[n][1],&chs,2);
				}
				ntot+=nread;
			}
			exit(ntot);
		}
		
		
	}
	/* processo padre */
	//chiudo tutte le pipe in lettura e scrittura tranne la i-esima pipe, aperta in scrittura
	for(int k = 0; k < N; k++){
		close(pipedF[k][1]);
	}
	
	char chs[2];    // buffer che verra' letto da file
	int nread;              // variabile che memorizzera' il ritorno della read
	char terminator=0;
	while(!terminator){
		terminator=1;
		for(int i=0;i<N;i++){
			nread = read(pipedF[i][0], chs, 2);/* ciclo di lettura fino a che nread e' maggiore di 0 */	
			if(nread!=0){
				terminator=0;
				printf("DEBUG:scrivo i car %c %c\n",chs[0],chs[1]);
				write(fdw,&chs,nread);	
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