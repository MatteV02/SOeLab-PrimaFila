#include <stdio.h>	// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close, fork, exec, read, write
#include <fcntl.h>	// Includo la libreria per la funzione open e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <stdbool.h>
#include <string.h>

//definisco il tipo pipe_t
typedef int pipe_t[2];

struct dato_pipe
{
	long int PC;
	char C;
} typedef dato_pipe;


int main(int argc, char** argv) {
	int N = argc - 3;	// numero di file
	char C1, C2;			// caratteri passati
	pipe_t* pipedNP;	// pipe per la comunicazione da nipoti a padre
	int pidFiglio;		// memorizzo il valore di ritorno della funzione fork nella generazione del figlio o memorizzo il ritorno della wait nel processo padre
	pipe_t pipedFN;		// pipe per la comunicazione da figlio a nipoto
	int pidNipote;		// variabile per memorizzare il pid del nipote ritornato dalle funzioni fork e wait
	int fd = 0; 		/*variabile che conterra il file descriptor dei file aperti dai processi figli e nipoti */
	int occorrenzeTrovate = 0;	// variabile che conterà le occorrenze dei caratteri trovati dai processi figli e nipoti
	long int pos = 0;	// variabile che memorizzerà la posizione delle occorrenze nei processi figli e nipoti
	int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
	int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
	dato_pipe occorrenza;	// struttura che memorizzarà le occorrenze mandate al padre e le occorrenze trovate dal figlio
	char c;	// variabile per memorizzare i caratteri letti da file

	if (argc < 2 + 1) /* controllo sul numero di parametri: devono essere in numero uguale a 2*/
	{
		printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
		exit(1);
	}

	if (argc == 2 + 1) /* controllo sul numero di parametri: devono essere in numero maggiore a 2*/
	{
		printf("Non faccio nessuna operazione dato che argc = %d\n", argc);
		exit(0);
	}

	//controlla che la stringa argv[argc - 2] sia un singolo carattere
	if (strlen(argv[argc - 2]) != 1) {
		printf("Errore, la stringa %s non e' un singolo carattere\n", argv[argc - 2]);
		exit(4);
	}
	C1 = argv[argc - 2][0];

	//controlla che la stringa argv[argc - 1] sia un singolo carattere
	if (strlen(argv[argc - 1]) != 1) {	// 
		printf("Errore, la stringa %s non e' un singolo carattere\n", argv[argc - 1]);
		exit(5);
	}
	C2 = argv[argc - 1][0];

	//apro N pipes
	pipedNP = (pipe_t*)malloc(sizeof(pipe_t) * (N));
	if (pipedNP == NULL) {
		printf("Errore nell'allocazione della memoria\n");
		exit(2);
	}
	
	for(int i = 0;i < N;i++){
		if(pipe(pipedNP[i])<0){
			printf("Errore nella creazione della pipe numero:%i\n",i);
			exit(2);
		}
	}
	
	for (int i = 0; i < N; i++) {
		
		if ((pidFiglio = fork()) < 0)	/* Il processo padre crea un figlio */
		{	/* In questo caso la fork e' fallita */
			printf("Errore durante la fork\n");
			exit(3);
		}
		
		if (pidFiglio == 0)
		{	/* processo figlio */
			//controllo se il file e' accedibile
			
			if (pipe(pipedFN) < 0 )
			{	printf("Errore creazione pipe nel figlio %d\n", i);
				exit(254);
			}
			
			if ((pidNipote = fork()) < 0)	/* Il processo padre crea un figlio */
			{	/* In questo caso la fork e' fallita */
				printf("Errore durante la fork\n");
				exit(253);
			}
			
			if (pidNipote == 0)
			{	/* processo nipote */
				dato_pipe occorrenzaFiglio;		// variabile usata per memorizzare l'occorrenza trovata dalla funzione figlia

				//chiudo tutte le pipe in lettura e scrittura tranne la i-esima pipe, aperta in scrittura
				close(pipedFN[1]);
				for(int k = 0; k < N; k++){
					close(pipedNP[k][0]);
					if (k != i) {
						close(pipedNP[k][1]);
					}
				}

				//controllo se il file e' accedibile
				if((fd = open(argv[i + 1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
					printf("Errore in apertura file %s dato che fd = %d\n", argv[i + 1], fd);
					exit(255);
				}

				while (read(pipedFN[0], &occorrenzaFiglio, sizeof(occorrenzaFiglio)) != 0)
				{
					
					while (read(fd, &c, 1) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
					{
						if (c == C2) {
							occorrenzeTrovate++;
							if (pos < occorrenzaFiglio.PC) {
								dato_pipe occorrenzaNipote = {pos, c};	// creo la struttura da mandare al padre
								write(pipedNP[i][1], &occorrenzaNipote, sizeof(occorrenzaNipote));
							} else {
								write(pipedNP[i][1], &occorrenzaFiglio, sizeof(occorrenzaFiglio));
							}
							pos++;
							break;
						}
						pos++;
					}
				}
				
				exit(occorrenzeTrovate);
			}
			/* processo figlio */

			close(pipedFN[0]);
			for (int i = 0; i < N; i++)
			{
				close(pipedNP[i][0]);
				close(pipedNP[i][1]);
			}

			if((fd = open(argv[i + 1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
				printf("Errore in apertura file %s dato che fd = %d\n", argv[i + 1], fd);
				exit(255);
			}
				
			while (read(fd, &c, 1) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
			{
				if (c == C1) {
					occorrenzeTrovate++;
					occorrenza.PC = pos; occorrenza.C = C1;	// creo la struttura da mandare al processo nipote

					write(pipedFN[1], &occorrenza, sizeof(occorrenza));
				}
				pos++;
			}

			close(pipedFN[1]);

			if ((pidNipote = wait(&status)) < 0) {
				printf("Non e' stato creato nessun processo figlio\n");
				exit(252);
			}
			
			if ((status & 0xFF) != 0)
			{
				printf("Il processo figlio è stato terminato in modo anomalo\n");
			} else {
				ritorno = (status >> 8) & 0xFF;
				printf("il nipote pid=%i ed ha ritornato=%i\n",pidNipote,ritorno);
			}

			exit(occorrenzeTrovate);
		}
		/* processo padre */
		
	}

	for (int i = 0; i < N; i++)
	{
		close(pipedNP[i][1]);
	}

	for (int i = 0; i < N; i++) {
		while (read(pipedNP[i][0], &occorrenza, sizeof(occorrenza)) > 0) {
			printf("Trovata una occorrenza del carattere %c nel file %s in posizione %ld\n", occorrenza.C, argv[i + 1], occorrenza.PC);
		}
	}

	for (int i = 0; i < N; i++)
	{
		if ((pidFiglio = wait(&status)) < 0) {
			printf("Non e' stato creato nessun processo figlio\n");
			exit(7);
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