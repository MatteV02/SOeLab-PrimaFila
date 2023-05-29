#include <stdio.h>		// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>		// Includo la libreria per la funzione exit
#include <unistd.h>		// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe,
#include <fcntl.h>		// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <string.h>
#include <errno.h>


struct struttura {
	long int c1;
	int c2;
	long int c3;
} typedef struttura;

//definisco il tipo pipe_t
typedef int pipe_t[2];

int main(int argc, char** argv) {
	char Cx;	// variabile che conterrà il singolo carattere presente come ultimo parametro passato
	int N;		// numero di file passati
	int pidFiglio;				// memorizza il valore di ritorno della funzione fork e wait
	int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
	int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio

	if (argc < 3 + 1) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 3*/
	{
		printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
		exit(1);
	}

	//controlla che la stringa argv[argc - 1] sia un singolo carattere
	if (strlen(argv[argc - 1]) != 1) {	
		printf("Errore, la stringa %s non e' un singolo carattere\n", argv[argc - 1]);
		exit(2);
	}
	Cx = argv[argc - 1][0];	// mi salvo il carattere nella variabile Cx

	N = argc - 2;			// memorizzo il numero di file passati

	struttura s = {-1,-1,0};	// inizializzo la struttura usata per il passaggio dei dati attraverso pipe

	//apro N + 1 pipes
	pipe_t* piped = (pipe_t*)malloc(sizeof(pipe_t) * (N + 1));
	if (piped == NULL) {
		printf("Errore nell'allocazione della memoria\n");
		exit(3);
	}
	
	for(int i = 0; i < N + 1; i++){
		if(pipe(piped[i])<0){
			printf("Errore nella creazione della pipe numero:%i\n",i);
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
			printf("DEBUG:figlio %d\n", i);
			int fd = 0; 					/*variabile che conterra il file descriptor del file che stiamo per aprire */
			char c;							// variabile usata per estrarre carattere per carattere il contenuto del file aperto
			long int contaOccorrenze = 0;	// variabile usata per contare le occorrenze del carattere Cx nel file aperto

			//chiudo tutte le pipe in lettura e scrittura tranne la i-esima pipe aperta in lettura e la i + 1-esima pipe aperta in scrittura
			for(int k = 0; k < N; k++){
				if (k != i) {
					close(piped[k][0]);
					close(piped[k + 1][1]);
				}
			}

			//apro il file argv[i + 1]
			if((fd = open(argv[i + 1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
				printf("Errore in apertura file %s dato che fd = %d\n", argv[i + 1], fd);
				write(piped[i + 1][1], &s, sizeof(s));	// mando al processo successivo la struttura passata dal processo precedente
				exit(255);
			}

			printf("DEBUG:figlio %i, file aperto\n", i);
			
			while (read(fd, &c, sizeof(char)) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
			{
				if (c == Cx) {
					contaOccorrenze++;	// trovata un occorrenza del carattere Cx, quindi incremento il contatore delle occorrenze
				}
			}

			printf("DEBUG:figlio %d trovate %ld occorrenze\n", i, contaOccorrenze);

			if (!read(piped[i][0], &s, sizeof(s))) {	// leggo la struttura mandata dal processo precedente nel ring. Per sicurezza controllo che la read non torni 0. In quel caso infatti il processo precedente si è chiuso in modo anomalo
				printf("Errore, la pipe %d e' stata chiusa prematuramente\n", i); 
				exit(255);
			}

			printf("DEBUG:il figlio %d riceve c1=%ld, c2=%d, c3=%ld\n", i, s.c1, s.c2, s.c3);

			s.c3 += contaOccorrenze;	// incremento il conteggio delle occorrenze globali
			if (s.c1 < contaOccorrenze) {	// controllo se il file attuale ha il massimo delle occorrenze
				s.c1 = contaOccorrenze;		// sostituisco i campi che memorizzano il massimo delle occorrenze
				s.c2 = i;
			}

			write(piped[i + 1][1], &s, sizeof(s));	// mando al processo successivo la struttura modificata

			exit(i);	// ritorno i al padre
		}
		/* processo padre */
		
	}	
	//chiudo tutte le pipe in lettura e scrittura tranne la N-esima pipe aperta in lettura e la 0-esima pipe aperta in scrittura
	for(int k = 0; k < N + 1; k++){
		if (k != N) {
			close(piped[k][0]);
			close(piped[(k + 1) % (N + 1)][1]);
		}
	}

	write(piped[0][1], &s, sizeof(s));	// mando al primo figlio la struttura inizializzata in modo che i dati vengano sostituiti

	if (!read(piped[N][0], &s, sizeof(s))) {	// leggo dall'ultimo figlio la struttura tornata, controllando per sicurezza che la pipe di comunicazione non sia stata chiusa
		printf("Errore, la pipe %d e' stata chiusa prematuramente\n", N); 
		exit(5);
	}

	printf("Il valore massimo di occorrenze trovate e' %ld, corrispondente al processo figlio con numero d'ordine %d, il quale ha trattato il file %s\nSono state trovate in totale %ld occorrenze.\n", s.c1, s.c2, argv[s.c2 + 1], s.c3);

	printf("DEBUG:attendo\n");
	// attendo la terminazione dei processi figli e ne stampo PID e valore di ritorno
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