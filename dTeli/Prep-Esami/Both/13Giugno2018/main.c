#include <stdio.h>		// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>		// Includo la libreria per la funzione exit
#include <unistd.h>		// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe,
#include <fcntl.h>		// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <stdbool.h>
//definisco il tipo pipe_t
typedef int pipe_t[2];
struct dato{
	//pid
	int c1;
	//lungezza linea corrente
	int c2;
};
void scambia(struct dato* v1,struct dato* v2){
	struct dato temp;
	temp.c1=v1->c1;
	temp.c2=v1->c2;
	v1->c1=v2->c1;
	v1->c2=v2->c2;
	v2->c1=temp.c1;
	v2->c2=temp.c2;
}

void bubbleSort(struct dato v[], int dim)
{ int i; bool ordinato = false;
	while (dim>1 && !ordinato)
	{ 
		ordinato = true; /* hp: è ordinato */
		for (i=0; i<dim-1; i++)
			if (v[i].c2 < v[i+1].c2)
			{ 
				scambia(&v[i],&v[i+1]);
				ordinato = false;
			}
		dim--;
	}
}



int main(int argc, char** argv) {
	
	if (argc < 2 + 1) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 2*/
	{
		printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
		exit(-1);
	}
	int Y;		/*Dichiarazione varibile che conterra' argv[argc-1]*/
	//controllo utlizzando la funzione atoi se il argv[argc-1] e un numero strettamente positivo
	if((Y = atoi(argv[argc-1])) <= 0){
		printf("il parametro %s non un numero positivo\n",argv[argc-1]);
		exit(-1);
	}
	int N=argc-2;
	//apro N+1 pipes
	pipe_t* piped = (pipe_t*)malloc(sizeof(pipe_t) * (N+1));
	if (piped == NULL) {
		printf("Errore nell'allocazione della memoria\n");
		exit(-1);
	}
	
	for(int i = 0; i < N+1; i++){
		if(pipe(piped[i])<0){
			printf("Errore nella creazione della pipe numero:%d\n",i);
			exit(-1);
		}
	}
	for(int i=0; i<N;i++){
		int pid;	// memorizza il valore di ritorno della funzione fork
		
		if ((pid = fork()) < 0)	/* Il processo padre crea un figlio */
		{	/* In questo caso la fork e' fallita */
			printf("Errore durante la fork\n");
			exit(-1);
		}
		
		if (pid == 0)
		{	/* processo figlio */
			//chiudo tutte le pipe in lettura e scrittura tranne la i-esima pipe, aperta in lettura
			for(int k = 0; k < N+1; k++){
				
				if (k != i) {
					close(piped[k][0]);
				}
				if(k!=i+1){
					close(piped[k][1]);
				}
			}
			
			//apro il file argv[i+1]
			int fd = 0; // variabile che conterra' il file descriptor del file argv[i+1]
			if((fd = open(argv[i+1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire il file */
				printf("Errore in apertura file %s dato che fd = %d\n", argv[i+1], fd);
				exit(-1);
			}
			int linea=0;
			struct dato* arr=malloc(sizeof(struct dato)*(i+1));
			char buffer;	// carattere usato per leggere il contenuto del file
			
			while (read(fd, &buffer, sizeof(char)) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
			{
				linea++;
				if(buffer=='\n'){
					if(i!=0){
						read(piped[i][0],arr,sizeof(struct dato)*i);
					}
					arr[i].c1=getpid();
					arr[i].c2=linea;
					write(piped[i+1][1],arr,sizeof(struct dato)*(i+1));
					linea=0;
				}

			}
			exit(i);
		}
		
		
	}
	/*processo padre*/
	//chiudo tutte le pipe in lettura e scrittura tranne la i-esima pipe, aperta in scrittura
	for(int k = 0; k < N+1; k++){
		close(piped[k][1]);
		if(k!=N){
			close(piped[k][0]);
		}
	}
	struct dato* arr=malloc(sizeof(struct dato)*N);
	
	for(int i=0;i<Y;i++){
		read(piped[N][0],arr,sizeof(struct dato)*N);
		bubbleSort(arr,N);
		printf("la linea N=%i\n",i);
		for(int i=0;i<N;i++){
			printf("il figlio pid=%i lungezza=%i\n",arr[i].c1,arr[i].c2);
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