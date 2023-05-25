#include <stdio.h>	// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe, dup
#include <fcntl.h>	// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <string.h>
//definisco il tipo pipe_t
typedef int pipe_t[2];
struct{
	//numero massimo delle occorrenze nel processo
	long int c1; 
	//indice del processo
	int c2; 
	// somma delle occorrenze
	long int c3; 
}struttura;

int main(int argc, char** argv) {
	char Cx; // valore Cx da controllare
	pipe_t* piped; // puntatore per le n pipe
	int N; // numero di fasi
	int pidFiglio;	// memorizzo il valore di ritorno della funzione fork e della wait
	int* pids; // contiene i pid dei figli
	int fd = 0; /*variabile che conterra il file descriptor del file che stiamo per aprire */
	char buffer; // contiene i singoli caratteri letti
	long int occorrenze=0;
	int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
	int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
			
	if (argc < 3 + 1) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 3*/
	{
		printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
		exit(-1);
	}
	//controlla che la stringa argv[argc-1] sia un singolo carattere
	if (strlen(argv[argc-1]) != 1) {
		printf("Errore, la stringa %s non e' un singolo carattere\n", argv[argc-1]);
		exit(-1);
	}
	Cx = argv[argc-1][0];
	//apro argc-2 pipes
	piped = (pipe_t*)malloc(sizeof(pipe_t) * (argc-2));
	if (piped == NULL) {
		printf("Errore nell'allocazione della memoria\n");
		exit(-1);
	}
	N=argc-2;
	for(int i = 0;i < N;i++){
		if(pipe(piped[i])<0){
			printf("Errore nella creazione della pipe numero:%i\n",i);
			exit(-1);
		}
	}
	pids=malloc(sizeof(int)*N);
	for(int i=0;i<N;i++){ 
		if ((pidFiglio = fork()) < 0)	/* Il processo padre crea un figlio */
		{	/* In questo caso la fork e' fallita */
			printf("Errore durante la fork\n");
			exit(-1);
		}
		pids[i]=pidFiglio;
		if (pidFiglio == 0)
		{	/* processo figlio */
			//chiudo tutte le pipe in lettura e scrittura tranne la i-esima pipe e la i-1-esima pipe, aperta in scrittura
			for(int k = 0; k < N; k++){
				close(piped[k][0]);
				if (k == i || k==i-1) {
					continue;
				}
				close(piped[k][1]);
			}
			if(i!=0){
			read(piped[i-1][0],&struttura,sizeof(struttura));
			}

			//controllo se il file e' accedibile
			if((fd = open(argv[i+1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
				printf("Errore in apertura file %s dato che fd = %d\n", argv[i+1], fd);
				exit(-1);
			}
			
			while (read(fd, &buffer, 1) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
			{
				//cerco le occorrenze di Cx nel file
				if(Cx==buffer){
					occorrenze++;
				}
			}
			struttura.c3+=occorrenze;
			if(struttura.c1<occorrenze){
				struttura.c2=i;
				struttura.c1=occorrenze;
			}
			write(piped[i][1],&struttura,sizeof(struttura));
			exit(i);
		}
		
	}
	/* processo padre */
	//chiudo tutte le pipe in lettura e scrittura tranne la i-esima pipe, aperta in  lettura
	for(int k = 0; k < N; k++){
		close(piped[k][1]);
		if (k != N-1) {
			close(piped[k][0]);
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
			printf("il figlio pid=%i ed ha ritornato=%i\n",pidFiglio,ritorno);
		
		}
	}
	read(piped[N-1][0],&struttura,sizeof(struttura));
	printf("il figlio=%i nel file=%s ha trovato n=%li occorenze del carattere %c totale occorrenze=%li\n",pids[struttura.c2],argv[struttura.c2+1],struttura.c1,Cx,struttura.c3);
	
	exit(0);

}