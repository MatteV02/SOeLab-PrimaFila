#include <stdio.h>	// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe, dup
#include <fcntl.h>	// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
//definisco il tipo pipe_t
typedef int pipe_t[2];

int main(int argc, char** argv) {
    if (argc < 2 + 1) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 2*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(-1);
    }
    int N=argc-1;
	char *cur=malloc(1*N);
    //apro N pipes
    pipe_t* piped = (pipe_t*)malloc(sizeof(pipe_t) * (N+1));
    if (piped == NULL) {
        printf("Errore nell'allocazione della memoria\n");
        exit(-1);
    }
    
    for(int i = 0;i < N+1;i++){
        if(pipe(piped[i])<0){
            printf("Errore nella creazione della pipe numero:%i\n",i);
            exit(-1);
        }
    }
	//creo il file
		int fdOut = 0; /*variabile che conterra il file descriptor del file che stiamo per creare */
		if((fdOut= creat("Teli", 0640)) < 0){		/* ERRORE se non si riesce a creare il file */
			printf("Errore in creazione del file %s dato che fd = %d\n", "Teli", fdOut);
			exit(-1);
		}
	for(int n=0;n<N;n++){
		int pid;	// memorizzo il valore di ritorno della funzione fork
		
		if ((pid = fork()) < 0)	/* Il processo padre crea un figlio */
		{	/* In questo caso la fork e' fallita */
			printf("Errore durante la fork\n");
			exit(-1);
		}
		if (pid == 0)
		{	/* processo figlio */
			//chiudo tutte le pipe in lettura e scrittura tranne la i-esima pipe, aperta in scrittura
			for(int k = 0; k < N+1; k++){
				if (k != n) {
					close(piped[k][0]);
				}
				if(k !=n+1){
					close(piped[k][1]);

				}
			}
			//controllo se il file e' accedibile
			int fd = 0; /*variabile che conterra il file descriptor del file che stiamo per aprire */
			if((fd = open(argv[n+1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
				printf("Errore in apertura file %s dato che fd = %d\n", argv[n+1], fd);
				exit(-1);
			}
			char car;
			int contatore=0;
			int contatoreDisp=0;
			char ultimoDisp;
			while (read(fd, &car, 1) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
			{
				if(contatore%2!=0){	
					contatoreDisp++;	
				}

				if(contatoreDisp==n+1){
					if(n!=0){
						read(piped[n][0],cur,sizeof(cur));
					}
					cur[n]=car;
					ultimoDisp=car;
					printf("DEBUG:sonoqui %c\n",cur[n]);
					for(int i=0;i<N;i++){
						printf("%i-%c\n",i,cur[i]);
					}
					write(piped[n+1][1],cur,sizeof(cur));
					break;
				}
				contatore++;
			}
			exit(ultimoDisp);
		}
	}
	/* processo padre */
	//chiudo tutte le pipe in lettura e scrittura tranne la i-esima pipe, aperta in scrittura
	//controllo se il file e' accedibile

	for(int k = 0; k < N+1; k++){
		if (k != N) {
			close(piped[k][0]);
		}
		close(piped[k][1]);
	}
	int nread;              // variabile che memorizzera' il ritorno della read
	while ((nread = read(piped[N][0], cur, sizeof(cur))) > 0 )	/* ciclo di lettura fino a che nread e' maggiore di 0 */
	{
		printf("leggo i caratteri\n");
		for(int i=0;i<N;i++){
			printf("%i-%c\n",i,cur[i]);
			write(fdOut,&cur[i],sizeof(cur[i]));
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
			printf("il figlio pid=%i ed ha ritornato=%c\n",pidFiglio,ritorno);
		}
	}
	
    exit(0);
}