#include <stdio.h>		// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>		// Includo la libreria per la funzione exit
#include <unistd.h>		// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe,
#include <fcntl.h>		// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
//definisco il tipo pipe_t
typedef int pipe_t[2];
struct{
	//pid
	int c1;
	//secondo carattere
	char c2;
	//penutimo carattere
	char c3; 
}dato;
int main(int argc, char** argv) {
	if (argc != 2 + 1) /* controllo sul numero di parametri: devono essere in numero uguale a 2*/
	{
		printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
		exit(-1);
	}
	int L;		/*Dichiarazione varibile che conterra' argv[2]*/
	//controllo utlizzando la funzione atoi se il argv[2] e un numero strettamente positivo
	if((L = atoi(argv[2])) <= 0){
		printf("il parametro %s non un numero positivo\n",argv[2]);
		exit(-1);
	}

	//apro L pipes
	pipe_t* piped = (pipe_t*)malloc(sizeof(pipe_t) * (L));
	if (piped == NULL) {
		printf("Errore nell'allocazione della memoria\n");
		exit(-1);
	}
	
	for(int i = 0; i < L; i++){
		if(pipe(piped[i])<0){
			printf("Errore nella creazione della pipe numero:%d\n",i);
			exit(-1);
		}
	}
	int pid;
	for(int q=0;q<L;q++){
			// memorizza il valore di ritorno della funzione fork
		
		if ((pid = fork()) < 0)	/* Il processo padre crea un figlio */
		{	/* In questo caso la fork e' fallita */
			printf("Errore durante la fork\n");
			exit(-1);
		}
		
		if (pid == 0)
		{	/* processo figlio */
			//chiudo tutte le pipe in lettura e scrittura tranne la q-esima pipe, aperta in scrittura
			for(int k = 0; k < L; k++){
				close(piped[k][0]);
				if (k != q) {
					close(piped[k][1]);
				}
			}

			//apro il file argv[1]
			int fd = 0; // variabile che conterra' il file descriptor del file argv[1]
			if((fd = open(argv[1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire il file */
				printf("Errore in apertura file %s dato che fd = %d\n", argv[1], fd);
				exit(-1);
			}
			int Nlinea=1;
			int cont=0;
			char linea[250];	// carattere usato per leggere il contenuto del file
			while (read(fd, &linea[cont], sizeof(char)) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
			{
				if(linea[cont]=='\n'){
					linea[cont]=0;
					int a=q+1;
					if(Nlinea==a){	
						dato.c1=getpid();
						dato.c2=linea[1];
						dato.c3=linea[cont-2];
						write(piped[q][1],&dato,sizeof(&dato));
						break;
					}
					Nlinea++;
					cont=0;
				}
				else{
					cont++;
				}
			}

			exit(Nlinea);
		}
		
	}
	/* processo padre */
	//chiudo tutte le pipe in scrittura
	for(int k = 0; k < L; k++){
		close(piped[k][1]);
	}

	for(int i=0;i<L;i++){
		read(piped[i][0],&dato,sizeof(dato));
		if(dato.c2==dato.c3){
			printf("il figlio=%i di indice %i ha trovato penultimo=%c  secondo=%c \n",dato.c1,i,dato.c3,dato.c2);
		}
	}

	for(int i=0;i<L;i++){
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