#include <stdio.h>	// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe, dup
#include <fcntl.h>	// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <string.h>
//definisco il tipo pipe_t
typedef int pipe_t[2];
int main(int argc, char** argv) {
	if (argc < 1 + 1) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 1*/
	{
		printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
		exit(-1);
	}
	int C;		/*Dichiarazione varibile che conterra' argv[argc-1]*/
	//controllo utlizzando la funzione atoi se il argv[argc-1] e un numero strettamente positivo
	if((C = atoi(argv[argc-1])) <= 0){
		printf("il parametro %s non un numero positivo\n",argv[argc-1]);
		exit(-1);
	}
	if(C%2==0){
		printf("il parametro %i non dispari\n",C);
		exit(-1);
	}
	int N=argc-2;
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

	for(int i=0;i<N*2;i++){
		int pid;	// memorizzo il valore di ritorno della funzione fork
		if ((pid = fork()) < 0)	/* Il processo padre crea un figlio */
		{	/* In questo caso la fork e' fallita */
			printf("Errore durante la fork\n");
			exit(-1);
		}
		
		if (pid == 0)
		{	/* processo figlio */
			char* b=malloc(C);
			int nro=0;	
			if(i>=N){
				//controllo se il file e' accedibile
				//creo il file
				int len=strlen(argv[i+1-N]);
				char* fileName=malloc(len+10);
				strcat(fileName,argv[i+1-N]);
				strcat(fileName,".mescolato");

				int fcreato = 0; /*variabile che conterra il file descriptor del file che stiamo per creare */
				if((fcreato = creat(fileName, 0640)) < 0){		/* ERRORE se non si riesce a creare il file */
					printf("Errore in creazione del file %s dato che fd = %d\n", fileName, fcreato);
					exit(-1);
				}
				int fd = 0; /*variabile che conterra il file descriptor del file che stiamo per aprire */
				if((fd = open(argv[i+1-N], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
					printf("Errore in apertura file %s dato che fd = %d\n", argv[i+1], fd);
					exit(-1);
				}
				for(int k=0;k<N;k++){
					close(piped[k][1]);
				}

				int lung=lseek(fd,0L,SEEK_END);	
				int nblocchi=(lung/C)/2;						
				lseek(fd,nblocchi*C,SEEK_SET);
			    // variabile che memorizzera' il ritorno della read
				int nread=0;
				while ((nread = read(fd,b,C)) > 0 )	/* ciclo di lettura fino a che nread e' maggiore di 0 */
				{
					nro++;
					if(nread!=C){
						printf("Errore, errore nella lettura %i %i\n",nread,i); 
					}
					write(fcreato,b,C);
					read(piped[i-N][0],b,C);
					write(fcreato,b,C);	
					
				}
			}
			else{

				//controllo se il file e' accedibile
				int fd = 0; /*variabile che conterra il file descriptor del file che stiamo per aprire */
				if((fd = open(argv[i+1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
					printf("Errore in apertura file %s dato che fd = %d\n", argv[i+1], fd);
					exit(-1);
				}
				int lung=lseek(fd,0L,SEEK_END);
				int nblocchi=(lung/C)/2;
				lseek(fd,0L,SEEK_SET);

				for(int k=0;k<N;k++){
					close(piped[k][0]);
				}

				int nread=0;
				while ((nread = read(fd,b,C)) > 0 )	/* ciclo di lettura fino a che nread e' maggiore di 0 */
				{
					if(nread!=C){
						printf("Errore, errore nella lettura %i %i\n",nread,i);
					}
					write(piped[i][1],b,C);
					if(nblocchi<=nro){
						break;
					}
					nro++;
				}
				

			}		
			exit(nro);
		}
		
		
	}
	/* processo padre */

	for(int i=0;i<N;i++){
		close(piped[i][0]);
		close(piped[i][1]);
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
			printf("il figlio pid=%i ed ha ritornato=%i\n",pidFiglio,ritorno);	
		}
	}

	exit(0);
}