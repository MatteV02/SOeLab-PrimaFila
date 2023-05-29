#include <stdio.h>	// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe, dup
#include <fcntl.h>	// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <time.h>
#include <errno.h>
#include <string.h>
//definisco il tipo pipe_t
typedef int pipe_t[2];
struct{
	//pid nipote
	int c1;
	//numero linea
	int c2;
	//contenuto linea
	char c3[250];
}dato;
int mia_random(int n)
{
	int casuale;
	casuale = rand() % n;
	casuale++;
	return casuale;
}
int main(int argc, char** argv) {
	if (argc < 2 + 1) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 2*/
	{
		printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
		exit(-1);
	}
	int N = (argc-1)/2;
	//apro N pipes
	pipe_t* piped = (pipe_t*)malloc(sizeof(pipe_t) * (N));
	if (piped == NULL) {
		printf("Errore nell'allocazione della memoria\n");
		exit(-1);
	}
	
	for(int i = 0;i < N;i++){
		if(pipe(piped[i])<0){
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
			pipe_t pipeF;
			
			if (pipe(pipeF) < 0 )
			{	printf("Errore creazione pipe\n");
				exit(-1);
			}
			int pid;	// memorizzo il valore di ritorno della funzione fork
			
			if ((pid = fork()) < 0)	/* Il processo padre crea un figlio */
			{	/* In questo caso la fork e' fallita */
				printf("Errore durante la fork\n");
				exit(-1);
			}
			
			if (pid == 0)
			{	/* processo nipote */
				//chiudo tutte le pipe in lettura e scrittura 
				srand(time(NULL));
				for(int k = 0; k < N; k++){
					close(piped[k][0]);
					close(piped[k][1]);
				}
				close(pipeF[0]);
				int randValue=mia_random(atoi(argv[i*2+2]));
				printf("DEBUG:randV=%i\n",randValue);
				write(pipeF[1],&randValue,sizeof(randValue));
				//chiudo lo stdin
				close(0);
				//controllo se il file e' accedibile
				int fd = 0; /*variabile che conterra il file descriptor del file che stiamo per aprire */
				if((fd = open(argv[i*2+1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
					printf("Errore in apertura file %s dato che fd = %d\n", argv[i*2+1], fd);
					exit(-1);
				}
				//chiudo lo stdout
				close(1);
				//duplico la pipe lato scrittura
				dup(pipeF[1]);
				//chiudo il lato della pipe duplicato
				close(pipeF[1]);
				char param[4];
				sprintf(param,"-%i",randValue);
				execlp("head","head",param, (char*) 0); // in caso di lista, ricordati di 0 terminare la lista dei parametri!
				/* si esegue l'istruzione seguente SOLO in caso di fallimento */
				
				perror("errore esecuzione comando"); 
				exit(errno);
			}
			/* processo figlio */
			close(pipeF[1]);
			//chiudo tutte le pipe in lettura e scrittura tranne la i-esima pipe, aperta in scrittura
			for(int k = 0; k < N; k++){
				close(piped[k][0]);
				if (k != i) {
					close(piped[k][1]);
				}
			}

			char buffer;
			int randValue=0;
			int pidNipote;
			if ((pidNipote = wait((int*) 0)) < 0) {
				printf("Non e' stato creato nessun processo figlio\n");
				exit(-1);
			}
			read(pipeF[0],&randValue,sizeof(randValue));
			dato.c1=getpid();
			dato.c2=0;
			int lung=0;
			while (read(pipeF[0], &buffer, 1) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
			{
				if(buffer=='\n'){
					dato.c3[lung]=0;
					lung=0;
					write(piped[i][1],&dato,sizeof(dato));
					dato.c2++;
				}
				else
				{
					dato.c3[lung]=buffer;
					lung++;	
				}
			}	
			exit(randValue);
		}
		
		
	}
	/* processo padre */
	//chiudo tutte le pipe in scrittura
	for(int k = 0; k < N; k++){
		close(piped[k][1]);
	}
	char terminator=0;
	while(terminator==0){
		terminator=1;
		for(int i=0;i<N;i++){
			if(read(piped[i][0],&dato,sizeof(dato))>0){
				terminator=0;
				printf("struttura da Npid=%i della linea %i contenuto=%s\n",dato.c1,dato.c2,dato.c3);
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