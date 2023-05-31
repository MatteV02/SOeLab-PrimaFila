#include <stdio.h>		// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>		// Includo la libreria per la funzione exit
#include <unistd.h>		// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe,
#include <fcntl.h>		// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <ctype.h>
#include <string.h>


struct struttura
{
	int c1;
	long int c2;
} typedef struttura;

//definisco il tipo pipe_t
typedef int pipe_t[2];


int main(int argc, char** argv) {
	char Cx;	// variabile usata per memorizzare il singolo carattere passato come ultimo parametro
	int N = argc - 2;	// variabile usata per memorizzare il numero di percorsi di file passati come parametro
	struttura* strutture;	// array di strutture usato per la comunicazione attraverso pipe
	pipe_t* piped;	// array di pipe usate per la comunicazione a ring
	int pidFiglio;	// memorizza il valore di ritorno della funzione fork e wait
	int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
	int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
	
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
	Cx = argv[argc - 1][0];	// Memorizzo il singolo carattere nella variabile Cx
	if (!islower(Cx)) {	// controllo che il carattere Cx sia alfabetico minuscolo
		printf("Errore, il carattere %c non è un carattere minuscolo\n", Cx); 
		exit(2);
	}

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

	for (int i = 0; i < N; i++)
	{		
		if ((pidFiglio = fork()) < 0)	/* Il processo padre crea un figlio */
		{	/* In questo caso la fork e' fallita */
			printf("Errore durante la fork\n");
			exit(4);
		}
		
		if (pidFiglio == 0)
		{	/* processo figlio */
			int fd = 0; // variabile che conterra' il file descriptor del file argv[i + 1]
			char c;	// carattere usato per leggere il contenuto del file associato a questo figlio
			long int nOccorrenze = 0;	// variabile usata per contare il numero di occorrenze del carattere Cx nel file associato a questo figlio

			//chiudo tutte le pipe in lettura e scrittura tranne la i-esima pipe aperta in lettura e la (i + 1)%N-esima pipe aperta in lettura
			for(int k = 0; k < N; k++){
				if (k != i) {
					close(piped[k][0]);
					close(piped[(k + 1) % N][1]);
				}
			}

			//apro il file argv[i + 1]
			if((fd = open(argv[i + 1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire il file */
				printf("Errore in apertura file %s dato che fd = %d\n", argv[i + 1], fd);
				exit(255);
			}
			
			while (read(fd, &c, sizeof(char)) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
			{
				if (c == Cx) {	// trovata un'occorrenza del file Cx nel file
					nOccorrenze++;
				}
			}

			// alloco la memoria per le strutture da passare al figlio successivo, la dimensione dell'array è i + 1
			strutture = malloc((i + 1) * sizeof(*strutture));
			if (strutture == NULL) {
				printf("Errore nell'allocazione della memoria nel processo figlio di ordine %d\n", i); 
				exit(255);
			}

			if (i != 0) {	// leggo dal figlio precedente l'array mandato (nel caso il processo sia il figlio 0 salto questa operazione)
				if (read(piped[i][0], strutture, sizeof(*strutture) * i) != sizeof(*strutture) * i) {	// l'array mandato avrà dimensione i, faccio il controllo sul numero di byte mandati per sicurezza
					printf("Errore, il numero di byte ricevuti tramite pipe dal figlio %d e' sbagliato\n", i); 
					exit(255);
				}
			}

			//	inserisco nell'array le occorrenze trovate dal processo attuale
			strutture[i].c1 = i;
			strutture[i].c2 = nOccorrenze;

			write(piped[(i + 1) % N][1], strutture, sizeof(*strutture) * (i + 1));	// invio l'array di strutture al figlio successivo

			exit(i);
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
	
	// il processo padre alloca la memoria per leggere le strutture mandate dai figli
	strutture = malloc(sizeof(*strutture) * N);
	if (strutture == NULL) {
		printf("Errore, nell'allocazione della memoria nel processo padre\n"); 
		exit(5);
	}
	read(piped[0][0], strutture, sizeof(*strutture) * N);	// leggo le strutture dalla pipe 0

	for (int i = 0; i < N; i++)	// stampo le N strutture ricevute a schermo
	{
		printf("Struttura %i: c1=%d, c2=%ld\n", i, strutture[i].c1, strutture[i].c2);
	}
	
	for (int i = 0; i < N; i++)	// ciclo di wait dei figli e stampa del valore di ritorno del figlio su schermo
	{
		if ((pidFiglio = wait(&status)) < 0) {
			printf("Non e' stato creato nessun processo figlio\n");
			exit(6);
		}
		
		if ((status & 0xFF) != 0)
		{
			printf("Il processo figlio %d è stato terminato in modo anomalo\n", pidFiglio);
		} else {
			ritorno = (status >> 8) & 0xFF;
			printf("Il figlio di PID %d ha ritornato %d\n",pidFiglio,ritorno);
		}
	}


	exit(0);
}