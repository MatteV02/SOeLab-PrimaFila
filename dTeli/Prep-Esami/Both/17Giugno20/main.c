#include <stdio.h>		// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>		// Includo la libreria per la funzione exit
#include <unistd.h>		// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe,
#include <fcntl.h>		// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <string.h>
//definisco il tipo pipe_t
typedef int pipe_t[2];

int main(int argc, char** argv) {
	if (argc != 3 + 1) /* controllo sul numero di parametri: devono essere in numero uguale a 3*/
	{
		printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
		exit(-1);
	}

int B;		/*Dichiarazione varibile che conterra' argv[2]*/
//controllo utlizzando la funzione atoi se il argv[2] e un numero strettamente positivo
if((B = atoi(argv[2])) <= 0){
	printf("il parametro %s non un numero positivo\n",argv[2]);
	exit(-1);
}
char * F=malloc(sizeof(argv[1]));
strcpy(F,argv[1]);
int L;		/*Dichiarazione varibile che conterra' 3*/
//controllo utlizzando la funzione atoi se il 3 e un numero strettamente positivo
if((L = atoi(argv[3])) <= 0){
	printf("il parametro %s non un numero positivo\n",argv[3]);
	exit(-1);
}
	char *file=malloc(sizeof(F)+7);
	strcpy(file,F);
	strcat(file,".Chiara");
	//apro il file file
	int fdcreato = 0; // variabile che conterra' il file descriptor del file file
	if((fdcreato = open(file, O_WRONLY)) < 0){		/* ERRORE se non si riesce ad aprire il file */
		printf("Errore in apertura file %s dato che fd = %d\n", file, fdcreato);
		exit(-1);
	}
//apro B pipes
pipe_t* piped = (pipe_t*)malloc(sizeof(pipe_t) * (B));
if (piped == NULL) {
	printf("Errore nell'allocazione della memoria\n");
	exit(-1);
}

for(int i = 0; i < B; i++){
	if(pipe(piped[i])<0){
		printf("Errore nella creazione della pipe numero:%d\n",i);
		exit(-1);
	}
}
int blocchi=L/B;
for(int i=0;i<B;i++){
	int pid;	// memorizza il valore di ritorno della funzione fork
	
	if ((pid = fork()) < 0)	/* Il processo padre crea un figlio */
	{	/* In questo caso la fork e' fallita */
		printf("Errore durante la fork\n");
		exit(-1);
	}
	
	if (pid == 0)
	{	/* processo figlio */
		//chiudo tutte le pipe in lettura e scrittura tranne la i-esima pipe, aperta in scrittura
		for(int k = 0; k < B; k++){
			close(piped[k][0]);
			if (k != i) {
				close(piped[k][1]);
			}
		}
		//apro il file F
		int fd = 0; // variabile che conterra' il file descriptor del file F
		if((fd = open(F, O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire il file */
			printf("Errore in apertura file %s dato che fd = %d\n", F, fd);
			exit(-1);
		}
		lseek(fd,i*blocchi,SEEK_SET);
		char* buff=malloc(blocchi);    // buffer che verra' letto da file
		int nread;              // variabile che memorizzera' il ritorno della read
		nread = read(fd, buff, blocchi);/* ciclo di lettura fino a che nread e' maggiore di 0 */
		write(piped[i][1],&buff[blocchi-1],1);
		exit(nread);
	}
	
	
}
	/* processo padre */
	//chiudo tutte le pipe in scrittura
	for(int k = 0; k < B; k++){
		close(piped[k][1]);
	}
	char car=0;
	for(int i=0;i<B;i++){
		read(piped[i][0],&car,1);
		write(fdcreato,&car,1);

	}
	
	for(int i=0;i<B;i++){
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