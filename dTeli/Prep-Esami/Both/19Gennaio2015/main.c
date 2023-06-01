#include <stdio.h>		// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>		// Includo la libreria per la funzione exit
#include <unistd.h>		// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe,
#include <fcntl.h>		// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <string.h>
 //definisco il tipo pipe_t
 typedef int pipe_t[2];
struct{
	int pid;
	long int occmax;
	long int occtotale;
}dato;

int main(int argc, char** argv) {
	if (argc < 4 + 1) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 4*/
	{
		printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
		exit(-1);
	}
	int N2=argc-3;
	if(N2%2!=0 || N2<2){
		printf("Errore, i file non sono pari o maggiori di 2\n"); 
		exit(-1);
	}	
	int N=N2/2;
	//controlla che la stringa argv[argc-1] contenga un singolo carattere
	if (strlen(argv[argc-1]) != 1) {
		printf("Errore, la stringa %s non e' un singolo carattere\n", argv[argc-1]);
		exit(-1);
	}
	char C2 = argv[argc-1][0];	// Memorizzo il singolo carattere nella variabile C1
	//controlla che la stringa argv[argc-2] contenga un singolo carattere
	if (strlen(argv[argc-2]) != 1) {
		printf("Errore, la stringa %s non e' un singolo carattere\n", argv[argc-2]);
		exit(-1);
	}
	char C1 = argv[argc-2][0];	// Memorizzo il singolo carattere nella variabile C2

	//apro N2 pipes
	pipe_t* piped = (pipe_t*)malloc(sizeof(pipe_t) * (N2));
	if (piped == NULL) {
		printf("Errore nell'allocazione della memoria\n");
		exit(-1);
	}
	
	for(int i = 0; i < N2; i++){
		if(pipe(piped[i])<0){
			printf("Errore nella creazione della pipe numero:%d\n",i);
			exit(-1);
		}
	}



	printf("DEBUG:N=%i N2=%i C1=%c C2=%c\n",N,N2,C1,C2);
	for(int i=0;i<N;i++){
		int pidFiglio;	// memorizza il valore di ritorno della funzione fork
		
		if ((pidFiglio = fork()) < 0)	/* Il processo padre crea un figlio */
		{	/* In questo caso la fork e' fallita */
			printf("Errore durante la fork\n");
			exit(-1);
		}
		
		if (pidFiglio == 0)
		{	/* processo figlio */
			//chiudo tutte le pipe in lettura e scrittura tranne la i-esima pipe, aperta in lettura
			for(int k = 0; k < N2; k++){
				if (k!=i+1) {
					//printf("DEBUG:chiudo la pipe %i in lettura %i\n",k,i);
					close(piped[k][0]);
				}

				if(k!=i){
					//printf("DEBUG:chiudo la pipe %i in scrittura %i\n",k,i);
					close(piped[k][1]);
				}
			}
			if(i==N-1){
				//printf("DEBUG:chiudo la pipe %i in lettura %i\n",N,i);
				close(piped[N][0]);
			}
			else{
			//	printf("DEBUG:ricevuto %i: pid=%i max=%li tot=%li\n",i,dato.pid,dato.occmax,dato.occtotale);
				read(piped[i+1][0],&dato,sizeof(dato));
			}

			//apro il file argv[i+1]
			int fd = 0; // variabile che conterra' il file descriptor del file argv[i+1]
			if((fd = open(argv[i+1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire il file */
				printf("Errore in apertura file %s dato che fd = %d\n", argv[i+1], fd);
				exit(-1);
			}

			char buffer;	// carattere usato per leggere il contenuto del file
			long int occ=0;
			while (read(fd, &buffer, sizeof(char)) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
			{
				if(buffer==C1){
					occ++;
				}
			}
			if(dato.occmax<occ){
				dato.occmax=occ;
				dato.pid=getpid();
			}
			dato.occtotale+=occ;
			write(piped[i][1],&dato,sizeof(dato));
			//printf("DEBUG:scritto %i: pid=%i max=%li tot=%li\n",i,dato.pid,dato.occmax,dato.occtotale);
			exit(0);
		}
			
	}
	for(int i=N;i<N2;i++){
		int pidFiglio;	// memorizza il valore di ritorno della funzione fork
		
		if ((pidFiglio = fork()) < 0)	/* Il processo padre crea un figlio */
		{	/* In questo caso la fork e' fallita */
			printf("Errore durante la fork\n");
			exit(-1);
		}
		
		if (pidFiglio == 0)
		{	/* processo figlio */

			for(int k = 0; k < N2; k++){
				if (k!=i+1) {
					//printf("DEBUG:chiudo la pipe %i in lettura %i\n",k,i);
					close(piped[k][0]);
				}

				if(k!=i){
					//printf("DEBUG:chiudo la pipe %i in scrittura %i\n",k,i);
					close(piped[k][1]);
				}
			}
			if(i==N2-1){
				//printf("DEBUG:chiudo la pipe %i in lettura %i\n",N,i);
				close(piped[N][0]);
			}else{
				read(piped[i+1][0],&dato,sizeof(dato));
			//	printf("DEBUG:ricevuto %i: pid=%i max=%li tot=%li\n",i,dato.pid,dato.occmax,dato.occtotale);
			}

			//apro il file argv[i+1]
			int fd = 0; // variabile che conterra' il file descriptor del file argv[i+1]
			if((fd = open(argv[i+1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire il file */
				printf("Errore in apertura file %s dato che fd = %d\n", argv[i+1], fd);
				exit(-1);
			}
			char buffer;	// carattere usato per leggere il contenuto del file
			long int occ=0;
			while (read(fd, &buffer, sizeof(char)) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
			{
				if(buffer==C2){
					occ++;
				}
			}

			if(dato.occmax<occ){
				dato.occmax=occ;
				dato.pid=getpid();
			}
			//printf("DEBUG:trovati %i %li\n",i,occ);
			dato.occtotale+=occ;
			write(piped[i][1],&dato,sizeof(dato));
			//printf("DEBUG:scritto %i: pid=%i max=%li tot=%li\n",i,dato.pid,dato.occmax,dato.occtotale);
			exit(0);
		}
	}
	/* processo padre */

	for(int k = 0; k < N2; k++){
		if (k != N && k!= 0) {
			close(piped[k][0]);
		}
		close(piped[k][1]);
		
	}
		read(piped[0][0],&dato,sizeof(dato));
		printf("pid=%i max=%li tot=%li\n",dato.pid,dato.occmax,dato.occtotale);		

		read(piped[N][0],&dato,sizeof(dato));
		printf("pid=%i max=%li tot=%li\n",dato.pid,dato.occmax,dato.occtotale);		

	for(int i=0;i<N2;i++){
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