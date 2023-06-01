#include <stdio.h>		// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>		// Includo la libreria per la funzione exit
#include <unistd.h>		// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe,
#include <fcntl.h>		// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <string.h>
#include <ctype.h>
#include <errno.h>
//definisco il tipo pipe_t
typedef int pipe_t[2];

int main(int argc, char** argv) {
	if (argc < 2 + 1) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 2*/
	{
		printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
		exit(-1);
	}
	if((argc-1)%2!=0){
		printf("Errore: numero di parametri non pari argc = %d\n", argc-1);
		exit(-1);		
	}
	int N=((argc-1)/2);
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
			pipe_t pipeF;
			
			if (pipe(pipeF) < 0 )
			{	printf("Errore creazione pipe\n");
				exit(-1);
			}
			int pidC;	// memorizza il valore di ritorno della funzione fork
			
			if ((pidC = fork()) < 0)	/* Il processo padre crea un figlio */
			{	/* In questo caso la fork e' fallita */
				printf("Errore durante la fork\n");
				exit(-1);
			}
			
			if (pidC == 0)
			{	/* processo nipote */
				close(pipeF[0]);
				// chiudo lo stdout
				close(1);
				// duplico la pipe lato  scrittura
				dup(pipeF[1]);
				// chiudo il lato della pipe duplicato
				close(pipeF[1]);
				
				char* valore=malloc(sizeof(argv[(i*2)+1])+1);
				strcat(valore,"-");
				strcat(valore,argv[(i*2+2)]);
				execlp("head","head",valore,argv[(i*2)+1],0); // in caso di lista, ricordati di 0 terminare la lista dei parametri! (char*)0
				/* si esegue l'istruzione seguente SOLO in caso di fallimento */
				
				perror("errore esecuzione comando");
				exit(errno);
				exit(-1);
			}
			/* processo figio */
			close(pipeF[1]);
			char buffer[250];	// carattere usato per leggere il contenuto del file
			int e=0;
			int linea=0;
			while (read(pipeF[0], &buffer[e], sizeof(char)) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
			{
				if(buffer[e]=='\n'){	
					buffer[e]=0;
					if(linea==(atoi(argv[i*2+2])-1))
					{
						break;
					}
					linea++;
					e=0;
				}
				else{
					e++;
				}		
			}
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
				if(ritorno!=0){
					ritorno=-1;
				}
			}

			//chiudo tutte le pipe in lettura e scrittura tranne la i-esima pipe, aperta in scrittura
			for(int k = 0; k < N; k++){
				close(piped[k][0]);
				if (k != i) {
					close(piped[k][1]);
				}
			}
			write(piped[i][1],&buffer,e+1);
			exit(ritorno);

		}
			


			
	}
		/* processo padre */
		for(int k=0;k<N;k++){
			close(piped[k][1]);
		}

		for(int i=0;i<N;i++){
			char buffer[250];	// carattere usato per leggere il contenuto del file
			int c=0;
			while (read(piped[i][0], &buffer[c], sizeof(char)) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
			{
				if(buffer[c]==0){
					printf("il figlio %i nel file %s ha trovato la linea %s %s\n",i,argv[i*2+1],argv[i*2+2],buffer);
					break;
				}
				c++;
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
				printf("Il figlio di PID %d ha ritornato %d\n",pidFiglio,ritorno);
			}
		}
	
		exit(0);
	}
	