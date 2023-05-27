#include <stdio.h>	// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe,
#include <fcntl.h>	// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <errno.h>


//definisco il tipo pipe_t
typedef int pipe_t[2];

int main(int argc, char** argv) {
	int N = argc - 2;	// memorizzo il numero di file passati
	int H;		/*Dichiarazione varibile che conterra' il numero intero strettamente positivo argv[argc - 1]*/
	int pidFiglio;	// memorizza il valore di ritorno della funzione fork e della funzione wait
	pipe_t* pipedFP;	// vettore di pipe per la comunicazione tra figli e padre
	int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
	int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
	char outputFigli[12];	// vettore di char che memorizzerà quanto passato dal processo figlio
	
	if (argc > 4 + 1) /* controllo sul numero di parametri: devono essere in numero minore o uguale a 4*/
	{
		printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
		exit(1);
	}

	//controllo utlizzando la funzione atoi se il argv[argc - 1] e un numero strettamente positivo
	if((H = atoi(argv[argc - 1])) <= 0){
		printf("il parametro %s non un numero positivo\n",argv[argc - 1]);
		exit(2);
	}

	//apro N pipes
	pipedFP = (pipe_t*)malloc(sizeof(pipe_t) * (N));
	if (pipedFP == NULL) {
		printf("Errore nell'allocazione della memoria\n");
		exit(3);
	}
	
	for(int i = 0;i < N;i++){
		if(pipe(pipedFP[i])<0){
			printf("Errore nella creazione della pipe numero:%i\n",i);
			exit(3);
		}
	}

	for (int i = 0; i < N; i++)	// genero N processi figli
	{		
		if ((pidFiglio = fork()) < 0)	/* Il processo padre crea un figlio */
		{	/* In questo caso la fork e' fallita */
			printf("Errore durante la fork\n");
			exit(4);
		}
		
		if (pidFiglio == 0)
		{	/* processo figlio */
			pipe_t pipedNF;	// variabile che conterrà gli estremi della pipe per la comunicazione tra nipoti e figli
			int pidNipote;	// memorizza il valore di ritorno della funzione fork e della wait relative al processo nipote
			char c;	// variabile usata per leggere un carattere alla volta dalla pipe
			char linea[256] = "";	// variabile che contiene la linea restituita dal comando wc: suppongo la linea sia lunga al massimo 256 caratteri
			int linea_size = 0;	// variabile che memorizza la lunghezza della linea
			int lunghezza;		/*Dichiarazione varibile che conterra' la lunghezza in linee del file*/
			
			//chiudo tutte le pipe in lettura e scrittura tranne la pipedFP-esima pipe, aperta in scrittura
			for(int k = 0; k < N; k++){
				close(pipedFP[k][0]);
				if (k != i) {
					close(pipedFP[k][1]);
				}
			}
			
			// creo la pipe per la comunicazione tra nipote e figlio
			if (pipe(pipedNF) < 0 )
			{	printf("Errore creazione pipe\n");
				exit(255);
			}
			
			if ((pidNipote = fork()) < 0)	/* Il processo figlio crea un nipote */
			{	/* In questo caso la fork e' fallita */
				printf("Errore durante la fork\n");
				exit(255);
			}
			
			if (pidNipote == 0)
			{	/* processo nipote */
				int fd = 0; /*variabile che conterra il file descriptor del file che stiamo per aprire */
				close(pipedNF[0]);

				close(1);	// ridireziono lo standard output per il comando wc sulla pipe tra nipote e padre
				dup(pipedNF[1]);
				close(pipedNF[1]);

				close(0);	// ridireziono lo standard output per il comando wc sul file associato al processo nipote
				//apro il file
				if((fd = open(argv[i + 1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
					printf("Errore in apertura file %s dato che fd = %d\n", argv[i + 1], fd);
					exit(255);
				}

				execlp("wc", "wc", "-l", (char*)0); // eseguo il comando "wc -l"
				/* si esegue l'istruzione seguente SOLO in caso di fallimento */
				
				perror("errore esecuzione comando");
				exit(errno);
			}
			/* processo figlio */
			
			close(pipedNF[1]);	// chiudo la pipe nipote-figli dal lato di scrittura
			
			while (read(pipedNF[0], &c, sizeof(char)) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
			{
				if (c == '\n') {
					break;	// nel caso incontri un capolinea, termino il ciclo di lettura
				}
				linea[linea_size++] = c;
			}
			linea[linea_size++] = 0;	// 0 termino la stringa

			//controllo utlizzando la funzione atoi se il linea e un numero strettamente positivo
			if((lunghezza = atoi(linea)) <= 0){
				printf("il valore %s non un numero positivo\n",linea);
				exit(255);
			}

			if (lunghezza >= H) {
				write(pipedFP[i][1], "Sopra media", 12 * sizeof(char));
			} else {
				write(pipedFP[i][1], "Sotto media", 12 * sizeof(char));
			}

			if ((pidNipote = wait(&status)) < 0) {
				printf("Non e' stato creato nessun processo figlio\n");
				exit(255);
			}
			
			if ((status & 0xFF) != 0)
			{
				printf("Il processo nipote è stato terminato in modo anomalo\n");
				exit(255);
			} else {
				ritorno = (status >> 8) & 0xFF;
			}

			exit(ritorno);
		}
		/* processo padre */
		
	}

	//chiudo tutte le pipe da figlio a padre in scrittura
	for(int k = 0; k < N; k++){
		close(pipedFP[k][1]);
	}

	for (int i = 0; i < N; i++)
	{

		if (read(pipedFP[i][0], outputFigli, sizeof(outputFigli)) != 0) {
			printf("Il file %s e' \"%s\"\n", argv[i + 1], outputFigli); 
		}
	}
	
	for (int i = 0; i < N; i++)
	{
		if ((pidFiglio = wait(&status)) < 0) {
			printf("Non e' stato creato nessun processo figlio\n");
			exit(6);
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