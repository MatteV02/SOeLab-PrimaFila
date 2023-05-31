#include <stdio.h>		// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>		// Includo la libreria per la funzione exit
#include <unistd.h>		// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe,
#include <fcntl.h>		// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <string.h>


struct struttura    // definisco la struttura che userò nella comunicazione tra processi
{
	int pid;
	long int occmax;
	long int occtotale;
} typedef struttura;


//definisco il tipo pipe_t
typedef int pipe_t[2];

int main(int argc, char** argv) {
	char C1, C2;    			// definisco le variabili che conterranno i singoli caratteri passati come parametro
	int N = (argc - 3) / 2;		// numero di parametri passati
	struttura s = {0, -1, 0};	// struttura usata per la comunicazione su pipe
	pipe_t* piped1;				// array di pipe usato per la comunicazione nella prima pipeline di processi
	pipe_t* piped2;				// array di pipe usato per la comunicazione nella seconda pipeline di processi
	int pidFiglio;				// memorizza il valore di ritorno della funzione fork e wait
	int fd = 0; 				// variabile che conterra' il file descriptor dei file associati ai processi figli
	char c;						// carattere usato per leggere il contenuto del file
	int curr = 0;				// contatore delle occorrenze dei caratteri nei processi figli
	int status;					// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
	int ritorno;				// La variabile usata per memorizzare il valore di ritorno del processo figlio

	if (argc < 4 + 1) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 4*/
	{
		printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
		exit(1);
	}

	if ((argc - 1) % 2 != 0) {
		printf("Errore, e' stato passato un numero dispari di parametri\n"); 
		exit(1);
	}

	//controlla che la stringa argv[argc - 1] contenga un singolo carattere
	if (strlen(argv[argc - 1]) != 1) {
		printf("Errore, la stringa %s non e' un singolo carattere\n", argv[argc - 1]);
		exit(2);
	}
	C2 = argv[argc - 1][0];	// Memorizzo il singolo carattere nella variabile C2

	//controlla che la stringa argv[argc - 2] contenga un singolo carattere
	if (strlen(argv[argc - 2]) != 1) {
		printf("Errore, la stringa %s non e' un singolo carattere\n", argv[argc - 2]);
		exit(2);
	}
	C1 = argv[argc - 2][0];	// Memorizzo il singolo carattere nella variabile C1

	//apro N pipes
	piped1 = (pipe_t*)malloc(sizeof(pipe_t) * (N));
	if (piped1 == NULL) {
		printf("Errore nell'allocazione della memoria\n");
		exit(3);
	}
	
	for(int i = 0; i < N; i++){
		if(pipe(piped1[i])<0){
			printf("Errore nella creazione della pipe numero:%d\n",i);
			exit(3);
		}
	}

	for (int n = 0; n < N; n++)	// avvio i processi della prima pipeline
	{		
		if ((pidFiglio = fork()) < 0)	/* Il processo padre crea un figlio */
		{	/* In questo caso la fork e' fallita */
			printf("Errore durante la fork\n");
			exit(4);
		}
		
		if (pidFiglio == 0)
		{	/* processo figlio */

			//chiudo tutte le pipe in lettura e scrittura tranne la n-esima pipe aperta in lettura e la (n - 1)-esima, aperta in scrittura (se n == 0, la pipe N - 1 è aperta in scrittura)
			for(int k = 0; k < N; k++){
				if (k != n) {
					close(piped1[k][0]);
					if (n != 0) {
						close(piped1[k - 1][1]);
					}
				}
			}

			//apro il file argv[n + 1]
			if((fd = open(argv[n + 1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire il file */
				printf("Errore in apertura file %s dato che fd = %d\n", argv[n + 1], fd);
				exit(255);
			}
			
			while (read(fd, &c, sizeof(char)) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
			{
				if (c == C1) {
					curr++;	// trovata un'occorrenza del carattere C1 quindi incremento curr
				}
			}

			if (n != N - 1) {
				read(piped1[n][0], &s, sizeof(s));	// leggo la struttura passata dal processo precedente (se questo esiste)
			}

			s.occtotale += curr;	// incremento il contatore delle occorrenze totali
			if (curr > s.occmax) {
				s.occmax = curr;	// se è il caso, aggiorno i dati relativi ai processi col numero massimo di occorrenze trovate
				s.pid = getpid();
			}

			// scrivo sulla pipe del processo precedente la struttura aggiornata (se il figlio è n == 0, scrivo sulla pipe che verrà letta dal padre)
			if (n == 0) {
				write(piped1[N - 1][1], &s, sizeof(s));
			} else {
				write(piped1[n - 1][1], &s, sizeof(s));
			}

			exit(0);
		}
		/* processo padre */
		
	}

	//chiudo tutte le pipe in lettura e scrittura di piped1 tranne la N - 1-esima pipe, aperta in lettura
	for(int k = 0; k < N; k++){
		close(piped1[k][1]);
		if (k != N - 1) {
			close(piped1[k][0]);
		}
	}

	//apro N pipes
	piped2 = (pipe_t*)malloc(sizeof(pipe_t) * (N));
	if (piped2 == NULL) {
		printf("Errore nell'allocazione della memoria\n");
		exit(3);
	}
	
	for(int i = 0; i < N; i++){
		if(pipe(piped2[i])<0){
			printf("Errore nella creazione della pipe numero:%d\n",i);
			exit(3);
		}
	}
	
	for (int n = 0; n < N; n++)
	{
		if ((pidFiglio = fork()) < 0)	/* Il processo padre crea un figlio */
		{	/* In questo caso la fork e' fallita */
			printf("Errore durante la fork\n");
			exit(4);
		}
		
		if (pidFiglio == 0)
		{	/* processo figlio */
			close(piped1[N - 1][0]);	// chiudo questa pipe che non verrà utilizzata

			//chiudo tutte le pipe in lettura e scrittura tranne la n-esima pipe aperta in lettura e la (n - 1)-esima, aperta in scrittura (se n == 0, la pipe N - 1 è aperta in scrittura)
			for(int k = 0; k < N; k++){
				if (k != n) {
					close(piped2[k][0]);
					if (n != 0) {
						close(piped2[k - 1][1]);
					}
				}
			}

			//apro il file argv[n + N + 1]
			if((fd = open(argv[n + N + 1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire il file */
				printf("Errore in apertura file %s dato che fd = %d\n", argv[n + N + 1], fd);
				exit(255);
			}
			
			while (read(fd, &c, sizeof(char)) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
			{
				if (c == C2) {
					curr++;	// trovata un'occorrenza del carattere C2 quindi incremento curr
				}
			}

			if (n != N - 1) {
				read(piped2[n][0], &s, sizeof(s));	// leggo la struttura passata dal processo precedente (se questo esiste)
			}

			s.occtotale += curr;	// incremento il contatore delle occorrenze totali
			if (curr > s.occmax) {
				s.occmax = curr;	// se è il caso, aggiorno i dati relativi ai processi col numero massimo di occorrenze trovate
				s.pid = getpid();
			}

			// scrivo sulla pipe del processo precedente la struttura aggiornata (se il figlio è n == 0, scrivo sulla pipe che verrà letta dal padre)
			if (n == 0) {
				write(piped2[N - 1][1], &s, sizeof(s));
			} else {
				write(piped2[n - 1][1], &s, sizeof(s));
			}

			exit(0);
		}
		/* processo padre */
		
	}

	//chiudo tutte le pipe in lettura e scrittura di piped2 tranne la N - 1-esima pipe, aperta in lettura
	for(int k = 0; k < N; k++){
		close(piped2[k][1]);
		if (k != N - 1) {
			close(piped2[k][0]);
		}
	}

	// leggo la struttura tornata dalla prima pipe
	if (read(piped1[N - 1][0], &s, sizeof(s)) == 0) {
		printf("Errore, la pipe 1 è stata chiusa in modo anomalo\n"); 
		exit(5);
	}

	printf("La prima pipeline, relativamente al carattere %c ha tornato una struttura con PID=%d, numero massimo di occorrenze trovate = %ld, numero totale di occorrenze trovate = %ld\n", C1, s.pid, s.occmax, s.occtotale);
	
	// leggo la struttura tornata dalla seconda pipe
	if (read(piped2[N - 1][0], &s, sizeof(s)) == 0) {
		printf("Errore, la pipe 2 è stata chiusa in modo anomalo\n"); 
		exit(6);
	}

	printf("La seconda pipeline, relativamente al carattere %c ha tornato una struttura con PID=%d, numero massimo di occorrenze trovate = %ld, numero totale di occorrenze trovate = %ld\n", C2, s.pid, s.occmax, s.occtotale);

	for (int i = 0; i < 2 * N; i++)	// ciclo di attesa dei processi figli e stampa dei valori di ritorno
	{
		if ((pidFiglio = wait(&status)) < 0) {
			printf("Non e' stato creato nessun processo figlio\n");
			exit(7);
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