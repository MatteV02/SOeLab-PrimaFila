#include <stdio.h>		// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>		// Includo la libreria per la funzione exit
#include <unistd.h>		// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe,
#include <fcntl.h>		// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
//definisco il tipo pipe_t
typedef int pipe_t[2];

int main(int argc, char** argv) {
	if (argc < 1 + 1) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 1*/
	{
		printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
		exit(-1);
	}
	//creo il file "Teli.log"
	int fcreato = 0; // variabile che conterra il file descriptor del file "Teli.log" */
	if((fcreato = creat("Teli.log", 0644)) < 0){		/* ERRORE se non si riesce a creare il file */
		printf("Errore in creazione del file %s dato che fcreato = %d\n", "Teli.log", fcreato);
		exit(-1);
	}
	int N=argc-1;
	//apro N*2 pipes
	pipe_t* piped = (pipe_t*)malloc(sizeof(pipe_t) * (N*2));
	if (piped == NULL) {
		printf("Errore nell'allocazione della memoria\n");
		exit(-1);
	}
	
	for(int i = 0; i < N*2; i++){
		if(pipe(piped[i])<0){
			printf("Errore nella creazione della pipe numero:%d\n",i);
			exit(-1);
		}
	}
	//apro N*2 pipes
	pipe_t* pipeS = (pipe_t*)malloc(sizeof(pipe_t) * (N*2));
	if (pipeS == NULL) {
		printf("Errore nell'allocazione della memoria\n");
		exit(-1);
	}
	
	for(int i = 0; i < N*2; i++){
		if(pipe(pipeS[i])<0){
			printf("Errore nella creazione della pipe numero:%d\n",i);
			exit(-1);
		}
	}

	for(int n=0;n<N*2;n++){
		int pid;	// memorizza il valore di ritorno della funzione fork
		int max=-1;
		if ((pid = fork()) < 0)	/* Il processo padre crea un figlio */
		{	/* In questo caso la fork e' fallita */
			printf("Errore durante la fork\n");
			exit(-1);
		}
	
		if (pid == 0)
		{	/* processo figlio */
			//chiudo tutte le pipe in lettura e scrittura tranne la n-esima pipe, aperta in scrittura
			for(int k = 0; k < N*2; k++){
				close(piped[k][0]);
				if (k != n) {
					close(piped[k][1]);
				}
			}
				//apro il file argv[i+1-N]
				int fd = 0; // variabile che conterra' il file descriptor del file argv[i+1-N]
				if((fd = open(argv[(n/2)+1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire il file */
					printf("Errore in apertura file %s dato che fd = %d\n", argv[n+1-N], fd);
					exit(-1);
				}
				//char linea[250];
				char buffer;	// carattere usato per leggere il contenuto del file
				int nro=0;
				int linea=1;
				while (read(fd, &buffer, sizeof(char)) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
				{
						if(buffer=='\n'){
							nro++;
							if(max==-1){
								max=nro;
								}
							if(max<nro){
								max=nro;
								}
						if(linea%2==0 && n%2==0){
							write(piped[n][1],&nro,sizeof(nro));
						}

						if(linea%2!=0 && n%2!=0){
							write(piped[n][1],&nro,sizeof(nro));
						}

						linea++;
						nro=0;
					}
					else{
						nro++;
					}
				}

			exit(max);
		}

		
	}
	/* processo padre */
	for(int k = 0; k < N*2; k++){
		close(piped[k][1]);
	}		
	int nro=0;
	char terminator=0;
	int contatore=0;
	for(int i=0;i<N;i++){
		contatore=1;
		terminator=0;
		printf("Le lunghezze delle linee del file %s sono:\n",argv[i+1]);
		while(terminator==0){
			terminator=1;
			int npipe=i*2;
			if(read(piped[npipe+1][0],&nro,sizeof(nro))>0){
				terminator=0;
				printf("linea numero %i e' lunga %i\n",contatore,nro);
				contatore++;
			}
			if(read(piped[npipe][0],&nro,sizeof(nro))>0){
				terminator=0;
				printf("linea numero %i e' lunga %i\n",contatore,nro);
				contatore++;
			}
			
		}
	}
	for(int i=0;i<N*2;i++){
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
			write(fcreato,&pidFiglio,sizeof(ritorno));
			char spazio=' ';
			char acapo='\n';
			write(fcreato,&spazio,1);
			write(fcreato,&ritorno,sizeof(ritorno));
			write(fcreato,&acapo,1);
			
		}
	}

	exit(0);
}