#include <stdio.h>		// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>		// Includo la libreria per la funzione exit
#include <unistd.h>		// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe,
#include <fcntl.h>		// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <string.h>
//definisco il tipo pipe_t
typedef int pipe_t[2];

int main(int argc, char** argv) {
	char ok=0;
	if (argc < 3 + 1) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 3*/
	{
		printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
		exit(-1);
	}
	//controlla che la stringa argv[1] contenga un singolo carattere
	if (strlen(argv[1]) != 1) {	
		printf("Errore, la stringa %s non e' un singolo carattere\n", argv[1]);
		exit(-1);
	}
	int N=argc-2;
	char Cx = argv[1][0];	// Memorizzo il singolo carattere nella variabile Cx
	//apro N pipes
	pipe_t* pipeD = (pipe_t*)malloc(sizeof(pipe_t) * (N));
	if (pipeD == NULL) {
		printf("Errore nell'allocazione della memoria\n");
		exit(-1);
	}
	
	for(int i = 0; i < N; i++){
		if(pipe(pipeD[i])<0){
			printf("Errore nella creazione della pipe numero:%d\n",i);
			exit(-1);
		}
	}

	//apro N pipes
	pipe_t* pipeS = (pipe_t*)malloc(sizeof(pipe_t) * (N));
	if (pipeS == NULL) {
		printf("Errore nell'allocazione della memoria\n");
		exit(-1);
	}
	
	for(int i = 0; i < N; i++){
		if(pipe(pipeS[i])<0){
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
				close(pipeD[k][0]);
				close(pipeS[k][1]);
				if (k != i) {
					close(pipeD[k][1]);
					close(pipeS[k][0]);
				}
			}
			//apro il file argv[i+1]
			int fd = 0; // variabile che conterra' il file descriptor del file argv[i+1]
			if((fd = open(argv[i+2], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire il file */
				printf("Errore in apertura file %s dato che fd = %d\n", argv[i+1], fd);
				exit(-1);
			}
			char buffer;	// carattere usato per leggere il contenuto del file
			long int pos=0;
			int occ=0;
			while (read(fd, &buffer, sizeof(char)) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
			{
				if(buffer==Cx){
					occ++;
					write(pipeD[i][1],&pos,sizeof(long int));
					read(pipeS[i][0],&ok,1);
					if(ok==1){
						printf("il figlio=%i di indice=%i ha trovato una occorrenza di %c in %li\n",getpid(),i,Cx,pos);
					}
				}
				pos++;
			}
			exit(occ);
		}
	}
	/* processo padre */
	//chiudo tutte le pipe in lettura e scrittura tranne la i-esima pipe, aperta in lettura
	for(int k = 0; k < N; k++){
		close(pipeD[k][1]);
		close(pipeS[k][0]);
	}
	long int max=0,pos;
	char terminator=0;
	char *terminati=malloc(N);
	int indice;
	while(terminator==0){
		terminator=1;
		for(int i=0;i<N;i++){
			if(read(pipeD[i][0],&pos,sizeof(long int))>0)
			{
				terminator=0;
				if(i==0){
					max=pos;
					indice=i;
				}
				else{
					if(pos>max){
						max=pos;
						indice=i;
					}
				}
			}else{

				terminati[i]=1;
			}
		}
		for(int i=0;i<N;i++){
			if(terminati[i]!=1){
				if(i==indice){
					ok=1;
				}else{
					ok=0;
				}
				write(pipeS[i][1],&ok,1);
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
			printf("Il processo figlio con PID %d è stato terminato in modo anomalo\n", pidFiglio);
		} else {
			ritorno = (status >> 8) & 0xFF;
			printf("Il figlio di PID %d ha ritornato %d\n",pidFiglio,ritorno);
		}
	}
	exit(0);
}
