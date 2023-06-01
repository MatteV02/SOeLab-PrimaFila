#include <stdio.h>		// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>		// Includo la libreria per la funzione exit
#include <unistd.h>		// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe,
#include <fcntl.h>		// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <string.h>


struct struttura    // definisco la struttura dati che usero' per la comunicazione su pipe
{
	int c1;     	// pid del processo attuale
	long int c2;    // occorrenze trovate dal processo attuale
} typedef struttura;

//definisco il tipo pipe_t
typedef int pipe_t[2];


int main(int argc, char** argv) {
	char C;             // variabile in cui memorizzero' il singolo carattere presente come ultimo parametro
	struttura* curr;    // vettore di strutture allocato dinamicamente usato per la comunicazione su pipe
	int N;              // variabile che conta il numero di file passati come parametro al programma
	pipe_t* piped;      // array di pipe allocato dinamicamente che contiene i riferimenti delle pipe per la comunicazione tra processi
	int pidFiglio;		// memorizza il valore di ritorno della funzione fork e wait
	int status;			// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
	int ritorno;		// La variabile usata per memorizzare il valore di ritorno del processo figlio

	
	if (argc < 3 + 1) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 3*/
	{
		printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
		exit(1);
	}

	//controlla che la stringa argv[argc - 1] contenga un singolo carattere
	if (strlen(argv[argc - 1]) != 1) {	
		printf("Errore, la stringa %s non e' un singolo carattere\n", argv[argc - 1]);
		exit(2);
	}
	C = argv[argc - 1][0];	// Memorizzo il singolo carattere nella variabile C

	N = argc - 2;	// il numero di file passati sara' pari a argc - 2

	//apro N pipes
	piped = (pipe_t*)malloc(sizeof(pipe_t) * (N));
	if (piped == NULL) {
		printf("Errore nell'allocazione della memoria\n");
		exit(3);
	}
	
	for(int i = 0; i < N; i++){
		if(pipe(piped[i])<0){
			printf("Errore nella creazione della pipe numero:%d\n",i);
			exit(3);
		}
	}

	for (int i = 0; i < N; i++) {        
		if ((pidFiglio = fork()) < 0)	/* Il processo padre crea un figlio */
		{	/* In questo caso la fork e' fallita */
			printf("Errore durante la fork\n");
			exit(4);
		}
		
		if (pidFiglio == 0)
		{	/* processo figlio */
			int fd = 0; // variabile che conterra' il file descriptor del file argv[i + 1]
			char ch;	// carattere usato per leggere il contenuto del file
			struttura attuale = {getpid(), 0};  // struttura che riempo coi dati trovati da questo processo figlio

			//chiudo tutte le pipe in lettura e scrittura tranne la i-esima pipe aperta in lettura e la (i + 1)%N-esima pipe aperta in scrittura
			for(int k = 0; k < N; k++){
				if (k != i) {
					close(piped[k][0]);
					close(piped[(k + 1) % N][1]);
				}
			}

			if (i == 0) {
				close(piped[0][0]);	// il figlio di ordine 0 non userà la pipe piped[0][0], quindi la chiudo
			}

			//apro il file argv[i + 1]
			if((fd = open(argv[i + 1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire il file */
				printf("Errore in apertura file %s dato che fd = %d\n", argv[i + 1], fd);
				exit(255);
			}
			
			while (read(fd, &ch, sizeof(char)) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
			{
				if (ch == C) {
					attuale.c2++;	// trovata un'occorrenza del carattere C nel file aperto, quindi incremento il contatore delle occorrenze
				}
			}

			// alloco la memoria per contenere le strutture prodotte dai processi precedenti e dal processo attuale
			curr = malloc(sizeof(*curr) * (i + 1));
			if (curr == NULL) {
				printf("Errore nell'allocazione della memoria nel figlio di ordine %i\n", i); 
				exit(255);
			}

			if (i != 0) {	// leggo le strutture prodotte dai processi precedenti solo se il figlio non è quello di ordine 0
				if (read(piped[i][0], curr, sizeof(*curr) * i) != sizeof(*curr) * i) {
					printf("Errore, figlio %d: il numero di byte letti dal processo precedente non e' quanto atteso\n", i); 
					exit(255);
				}
			}

			curr[i] = attuale;	// aggiungo all'array di strutture la struttura prodotta dal processo attuale

			write(piped[(i + 1) % N][1], curr, sizeof(*curr) * (i + 1));	// mando l'array di strutture al processo successivo

			exit(i);	// ritorno al padre l'indice d'ordine del processo attuale
		}
		/* processo padre */
		
	}

	//chiudo tutte le pipe in lettura e scrittura tranne la 0-esima pipe, aperta in lettura
	for(int k = 0; k < N; k++){
		close(piped[k][1]);
		if (k != 0) {
			close(piped[k][0]);
		}
	}

	// alloco l'array di strutture in modo da contenere le strutture prodotte dagli N processi figli
	curr = malloc(sizeof(*curr) * N);
	if (curr == NULL) {
		printf("Errore, nell'allocazione della memoria nel processo padre\n"); 
		exit(5);
	}

	// leggo le N strutture prodotte dai processi figli
	if (read(piped[0][0], curr, sizeof(*curr) * N) != sizeof(*curr) * N) {
		printf("Errore, padre: il numero di byte letti non è quanto atteso\n"); 
		exit(5);
	}

	for (int i = 0; i < N; i++)	// stampo le N strutture prodotte dagli N processi figli
	{
		printf("La struttura %i contiene c1=%d, c2=%ld\n", i, curr[i].c1, curr[i].c2);
	}
	
	for (int i = 0; i < N; i++)	// ciclo di wait dei processi figli e stampa dei loro valori di ritorno
	{
		if ((pidFiglio = wait(&status)) < 0) {
			printf("Non e' stato creato nessun processo figlio\n");
			exit(6);
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