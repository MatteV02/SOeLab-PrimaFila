#include <stdio.h>	// Includo la libreria per la funzione printf
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close, fork, read, write, pipe
#include <fcntl.h>	// Includo la libreria per la funzione open e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait


//definisco il tipo pipe_t
typedef int pipe_t[2];

int main(int argc, char** argv) {
	int K;		        /*Dichiarazione varibile che conterra' il numero intero presente in argv[argc - 1]*/
	int N = argc - 2;   // memorizzo in questa variabile il numero di file passati come parametro
	char input[11];     // stirnga in cui memorizzo l'input dell'utente. Se l'input supera i 10 caratteri ho un errore
	int input_size = 0; // variabile che memorizza la lunghezza dell'input inserito dall'utente
	char c;             // variabile utilizzata per leggere dai file carattere per carattere
	int X;              // variabile su cui verrà fatto il parsing dell'input dell'utente
	int *pidFigli;      // array dinamico di pid dei figli generati
	pipe_t* pipedFP;    // array di pipe per il dialogo da figli a padre
	int pidFiglio;	    // memorizza il valore di ritorno della funzione fork e wait
	char linea[256];    // array in cui memorizzo la linea. Suppongo ogni linea abbia una dimensione minore o uguale a 255 caratteri
	int L = 0;          // variabile in cui memorizzo la lunghezza della linea
	int status;	        // La variabile usata per memorizzare quanto ritornato dalla primitiva wait
	int ritorno;	    // La variabile usata per memorizzare il valore di ritorno del processo figlio

	if (argc < 3 + 1) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 3*/
	{
		printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
		exit(1);
	}

	//controllo utlizzando la funzione atoi se il argv[argc - 1] e un numero strettamente positivo
	if((K = atoi(argv[argc - 1])) <= 0){
		printf("il parametro %s non un numero positivo\n",argv[argc - 1]);
		exit(2);
	}
	
	// Richiedo all'utente un numero strettamente positivo e minore o uguale a K
	printf("Inserisci un numero tra 1 e %d:\n", K);
	
	while (read(0, &c, sizeof(char)) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da stdin */
	{
		if (input_size == 10) {	// se leggo più di 10 carattere l'input non può essere un numero intero a 32 bit
			printf("Errore, un numero a 32 bit non può essere digitato con una stringa di più di 10 caratteri!\n"); 
			while (read(0, &c, sizeof(char)) > 0) { // consumo il resto dell'input digitato
				if (c == '\n') {
					break;
				}
			}
			exit(3);
		}
		if (c == '\n') {
			// trovato caporiga
			input[input_size++] = 0;
			break;
		} else {
			// aggiungo carattere al buffer di input
			input[input_size++] = c;
		}
	}

	if (input_size == 0) {
		printf("Errore, non è stato inserito nulla\n"); 
		exit(3);
	}

	X = atoi(input);	// faccio il parsing dell'input digitato

	if (X <= 0 || X > K) {
		printf("Errore, il valore inserito non è un numero oppure non è strettamente positivo e minore o uguale a %d\n", K); 
		exit(3);
	}

	pidFigli = malloc(N * sizeof(*pidFigli));	// preparo il vettore che conterrà i pid dei figli generati
	if (pidFigli == NULL) {
		printf("Errore nell'allocazione della memoria\n");
		exit(4);
	}

	//apro N pipes
	pipedFP = (pipe_t*)malloc(sizeof(pipe_t) * (N));
	if (pipedFP == NULL) {
		printf("Errore nell'allocazione della memoria\n");
		exit(5);
	}
	
	for(int i = 0; i < N; i++){
		if(pipe(pipedFP[i])<0){
			printf("Errore nella creazione della pipe numero:%i\n",i);
			exit(5);
		}
	}

	for (int i = 0; i < N; i++)
	{        
		if ((pidFiglio = fork()) < 0)	/* Il processo padre crea un figlio */
		{	/* In questo caso la fork e' fallita */
			printf("Errore durante la fork\n");
			exit(6);
		}
		
		if (pidFiglio == 0)
		{	/* processo figlio */
			int fd = 0;         /*variabile che conterra il file descriptor del file associato al figlio attuale */
			int x = 0;          // contatore che conta le linee lette finora

			//chiudo tutte le pipe in lettura e scrittura tranne la i-esima pipe, aperta in scrittura
			for(int k = 0; k < N; k++){
				close(pipedFP[k][0]);
				if (k != i) {
					close(pipedFP[k][1]);
				}
			}

			//controllo se il file e' accedibile
			if((fd = open(argv[i + 1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
				printf("Errore in apertura file %s dato che fd = %d\n", argv[i + 1], fd);
				exit(255);
			}
			
			while (read(fd, &c, sizeof(char)) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
			{
				if (c == '\n') {
					// trovato caporiga
					linea[L++] = 0;	// zero-termino la stringa
					x++;	// incrementro il contatore di linea
					if (x == X) {
						// trovata la riga X-esima, quindi esco dal ciclo
						break;
					}
					L = 0;	// ripristino la lunghezza della linea per memorizzare i dati successivi
				} else {
					linea[L++] = c;	// aggiungo in coda alla linea il carattere letto
				}
			}

			write(pipedFP[i][1], &L, sizeof(L));	// mando al processo figlio la lunghezza della linea letta o 0 se la linea X-esima non è presente nel file
			if (x == X) {
				// linea e' stata trovata, quindi la mando al padre
				write(pipedFP[i][1], linea, L * sizeof(*linea));
			}

			exit(L);
		}
		/* processo padre */
		
		pidFigli[i] = pidFiglio;   // memorizzo il pid del figlio
	}
	
	//chiudo tutti i lati di scrittura delle pipe
	for(int k = 0; k < N; k++){
		close(pipedFP[k][1]);
	}

	// in questo ciclo leggo quanto mandato dai processi figli e stampo a schermo i risultati
	for (int i = 0; i < N; i++)
	{
		if (read(pipedFP[i][0], &L, sizeof(L)) == 0)	// leggo la lunghezza della linea letta
		{
			printf("Errore, la pipe del figlio %i e' stata chiusa prematuramente\n", i); 
			exit(7);
		}
		
		if (L > 0)
		{
			// la linea è stata trovata
			if (read(pipedFP[i][0], linea, sizeof(*linea) * L) == 0)	// leggo il contenuto della linea
			{
				printf("Errore, la pipe del figlio %i e' stata chiusa prematuramente\n", i); 
				exit(6);
			}
			printf("Il figlio con PID %d ha letto dal file %s la linea \"%s\"\n", pidFigli[i], argv[i + 1], linea);
		} else {
			// la linea non è stata trovata
			printf("Il figlio con PID %d ha determinato che il file %s non possiede una %d-esima linea\n", pidFigli[i], argv[i + 1], X);
		}
	}

	// in questo ciclo attendo che i processi figli terminino e stampo a schermo il loro PID ed il loro valore di ritorno
	for (int i = 0; i < N; i++)
	{
		if ((pidFiglio = wait(&status)) < 0) {
			printf("Non e' stato creato nessun processo figlio\n");
			exit(8);
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