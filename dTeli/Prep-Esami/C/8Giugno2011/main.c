#include <stdio.h>	// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe, dup
#include <fcntl.h>	// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <string.h>
//definisco il tipo pipe_t
typedef int pipe_t[2];
struct{
	long int PC;
	char C;
}occorenza;

int main(int argc, char** argv) {
	char buffer; //buffer di lettura file
	long int contatore=0; // contatore del file
	char C2;
	char C1;
	int fd = 0; /*variabile che conterra il file descriptor del file che stiamo per aprire */
	int N;
	int nread=0;
	pipe_t* pipedN;
	pipe_t* pipedP; 
	int nOccorenze=0;
	int pidNipote;	// memorizzo il valore di ritorno della funzione fork
	int pidF;	// memorizzo il valore di ritorno della fork per il figlio
	int pidNip;
	int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
	int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
	int pidFiglio;

	if (argc < 3 + 1) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 3*/
	{
		printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
		exit(1);
	}
	//controlla che la stringa argv[argc-1] sia un singolo carattere
	if (strlen(argv[argc-1]) != 1) {
		printf("Errore, la stringa %s non e' un singolo carattere\n", argv[argc-1]);
		exit(2);
	}
	C1 = argv[argc-1][0];

	//controlla che la stringa argv[argc-2] sia un singolo carattere
	if (strlen(argv[argc-2]) != 1) {	
		printf("Errore, la stringa %s non e' un singolo carattere\n", argv[argc-2]);
		exit(2);
	}
	C2 = argv[argc-2][0];
	N=argc-3;
	//apro N pipes per la comunicazione fra i figli e suoi nipoti
	pipedN = (pipe_t*)malloc(sizeof(pipe_t) * (N));
	if (pipedN == NULL) {
		printf("Errore nell'allocazione della memoria\n");
		exit(2);
	}
	
	for(int i = 0;i < N;i++){
		if(pipe(pipedN[i])<0){
			printf("Errore nella creazione della pipe numero:%i\n",i);
			exit(2);
		}
	}
	//apro N pipes per la comunicazione fra i nipoti e il padre
	pipedP = (pipe_t*)malloc(sizeof(pipe_t) * (N));
	if (pipedP == NULL) {
		printf("Errore nell'allocazione della memoria\n");
		exit(4);
	}
	
	for(int i = 0;i < N;i++){
		if(pipe(pipedP[i])<0){
			printf("Errore nella creazione della pipe numero:%i\n",i);
			exit(4);
		}
	}

	for(int i=0;i<N;i++){
		
		if ((pidF = fork()) < 0)	/* Il processo padre crea un figlio */
		{	/* In questo caso la fork e' fallita */
			printf("Errore durante la fork\n");
			exit(5);
		}
		
		if (pidF == 0)
		{	/* processo figlio */	
			if ((pidNipote = fork()) < 0)	/* Il processo padre crea un figlio */
			{	/* In questo caso la fork e' fallita */
				printf("Errore durante la fork\n");
				exit(6);
			}
			
			if (pidNipote == 0)
			{	/* processo Nipote */
				int nOccorenze=0;
				//chiudo tutte le pipe tra nipote padre in lettura e scrittura tranne la i-esima pipe, aperta in scrittura
				//chiudo tutte le pipe tra nipote figlio in lettura e scrittura tranne la i-esima pipe, aperta in lettura
				for(int k = 0; k < N; k++){
					close(pipedN[k][1]);
					close(pipedP[k][0]);
					if (k != i) {
						close(pipedN[k][0]);
						close(pipedP[k][1]);
					}
				}
				//controllo se il file e' accedibile
				if((fd = open(argv[i+1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
					printf("Errore in apertura file %s dato che fd = %d\n", argv[i+1], fd);
					exit(7);
				}
				while ((nread = read(pipedN[i][0],&occorenza, sizeof(occorenza))) > 0 )	/* ciclo di lettura fino a che nread e' maggiore di 0 */
				{
					while (read(fd, &buffer, 1) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
					{
						//
						if(buffer==C2){
							nOccorenze++;
							if(contatore<occorenza.PC){
								occorenza.C=C2;
								occorenza.PC=contatore;
								write(pipedP[i][1],&occorenza,sizeof(occorenza));
							}
							else{
								write(pipedP[i][1],&occorenza,sizeof(occorenza));
							}
							contatore++;
							break;
						}   
						contatore++;
					}
				}
				exit(nOccorenze);
			}
			/* processo Figlio */
			for(int k = 0; k < N; k++){
					close(pipedP[k][0]);
					close(pipedP[k][1]);
					close(pipedN[k][0]);
					if (k != i) {
						close(pipedN[k][1]);
					}
			}
			//controllo se il file e' accedibile
			if((fd = open(argv[i+1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
				printf("Errore in apertura file %s dato che fd = %d\n", argv[i+1], fd);
				exit(5);
			}

			while (read(fd, &buffer, 1) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
			{
				if(buffer==C1){
					occorenza.PC=contatore;
					occorenza.C=C1;
					nOccorenze++;
					write(pipedN[i][1],&occorenza,sizeof(occorenza));
				}
				contatore++;
			}
			close(pipedN[i][1]);
			if ((pidNip = wait(&status)) < 0) {
				printf("Non e' stato creato nessun processo figlio\n");
				exit(7);
			}
			
			if ((status & 0xFF) != 0)
			{
				printf("Il processo figlio è stato terminato in modo anomalo\n");
			} else {
				ritorno = (status >> 8) & 0xFF;
				printf("il nipote pid=%i ed ha ritornato=%i\n",pidNip,ritorno);
			}
			exit(nOccorenze);
		}
	}
	/* processo padre */
	//chiudo tutte le pipe in lettura e scrittura tranne la i-esima pipe, aperta in scrittura
	for(int k = 0; k < N; k++){
		close(pipedN[k][1]);
		close(pipedN[k][0]);
		close(pipedP[k][1]);
	}

	for(int i=0;i<N;i++){
		while(read(pipedP[i][0],&occorenza,sizeof(occorenza))>0){
			printf("file N%i in pos %li ha il car %c\n",i,occorenza.PC,occorenza.C);
		}
	}
	for(int i=0;i<N;i++){
		if ((pidFiglio = wait(&status)) < 0) {
			printf("Non e' stato creato nessun processo figlio\n");
			exit(9);
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