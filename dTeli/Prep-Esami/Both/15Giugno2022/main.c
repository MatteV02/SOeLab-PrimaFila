#include <stdio.h>		// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>		// Includo la libreria per la funzione exit
#include <unistd.h>		// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe,
#include <fcntl.h>		// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <ctype.h>
//definisco il tipo pipe_t
typedef int pipe_t[2];

int main(int argc, char** argv) {

	int N=argc-1;	
	char ok;
	char *terminati=malloc(N);
	char terminator=0;
	char ch;
	int indice=0;
	char max=0;
	int pidFiglio;
	int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
	int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
	int pid;	// memorizza il valore di ritorno della funzione fork
	pipe_t* pipedD = (pipe_t*)malloc(sizeof(pipe_t) * (N));
	pipe_t* pipedS = (pipe_t*)malloc(sizeof(pipe_t) * (N));
	int fd = 0; // variabile che conterra' il file descriptor del file argv[i+1]
	long int cur=0;
	int stampe=0;
	
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
	
	for(int i = 0; i < N; i++){
		if(pipe(pipedD[i])<0){
			printf("Errore nella creazione della pipe numero:%d\n",i);
			exit(-1);
		}
	}
	//apro N pipes
	if (pipedS == NULL) {
		printf("Errore nell'allocazione della memoria\n");
		exit(-1);
	}
	
	for(int i = 0; i < N; i++){
		if(pipe(pipedS[i])<0){
			printf("Errore nella creazione della pipe numero:%d\n",i);
			exit(-1);
		}
	}

	for(int i=0;i<N;i++){
		
		
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
			//apro il file argv[i+1]
			
			if((fd = open(argv[i+1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire il file */
				printf("Errore in apertura file %s dato che fd = %d\n", argv[i+1], fd);
				exit(-1);
			}
			while (read(fd, &ch, sizeof(char)) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
			{
				if(islower(ch)){
					write(pipedD[i][1],&ch,sizeof(ch));
					read(pipedS[i][0],&ok,sizeof(ok));
					if(ok==1){
						printf("il caratttere trovato %c alla posizione %li del figlio=%i indice %i del file %s \n",ch,cur,getpid(),i,argv[i+1]);
						stampe++;
					}
				}
				cur++;
			}
			
			exit(stampe);
		}	
	}
	/* processo padre */
	//chiudo tutte le pipe in lettura e scrittura tranne la i-esima pipe, aperta in lettura
	for(int k = 0; k < N; k++){
		close(pipedD[k][1]);
		close(pipedS[k][0]);
	}
	while(terminator==0){
		terminator=1;
		max=-1;
		for(int i=0;i<N;i++){

			if(read(pipedD[i][0],&ch,sizeof(ch))>0){
				terminator=0;
				if(max==-1){
					max=ch;
					indice=i;
					continue;
				}
				if(max<ch){
					max=ch;
					indice=i;
				}
			}
			else{
				terminati[i]=1;
			}
		}
		for(int i=0;i<N;i++){
		if(terminati[i]!=1){
			if(i==indice){
				ok=1;
			}
			else{
				ok=0;
			}
			write(pipedS[i][1],&ok,sizeof(ok));
		}
		}
	}
	for(int i=0;i<N;i++){
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