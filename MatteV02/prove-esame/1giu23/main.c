#include <stdio.h>		// Includo la libreria per la funzione printf
#include <stdlib.h>		// Includo la libreria per la funzione exit
#include <unistd.h>		// Includo la libreria per la funzione close, fork, execlp, read, write, getpid, pipe
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <errno.h>		// Includo la libreria errno perché la variabile globale errno e' usata nel processo nipote in caso di fallimento del comando execlp
#include <string.h>     // Includo questa libreria per la funzione memcpy


struct struct_t	// definisco la struttura di dati usata per la comunicazione su pipe
{
	int c1;			// campo della struttura che conterra' il PID del figlio
	char c2[250];	// campo della struttura che conterra' l'ultima linea del file prodotta dal comando rev
	int c3;			// lunghezza dell'ultima linea con il terminatore di linea
} typedef struct_t;


//definisco il tipo pipe_t
typedef int pipe_t[2];

int main(int argc, char** argv) {
	int H;							// variabile che conterra' il numero di arametri passati al processo padre
	struct_t st;					// struttura usata per il passaggio di dati su pipe
	pipe_t* pipedFP;				// array di pipe che sara' allocato dinamicamente della dimensione pari a H
	int pidFiglio, pidNipote;		// memorizza il valore di ritorno della funzione fork e wait rispettivamente dei processi figli e dei processi nipoti
	int status;						// La variabile usata per memorizzare quanto ritornato dalla primitiva wait nel processo padre e nel processo figlio
	int ritorno;					// La variabile usata per memorizzare il valore di ritorno del processo figlio e del processo nipote

	if (argc < 2 + 1) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 2*/
	{
		printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
		exit(1);
	}

	H = argc -1;	// memorizzo il numero dei file passati come parametro

	//apro H pipes
	pipedFP = (pipe_t*)malloc(sizeof(pipe_t) * (H));
	if (pipedFP == NULL) {	// controllo che l'allocazione della memoria sia avvenuta correttamente
		printf("Errore nell'allocazione della memoria\n");
		exit(2);
	}
	
	for(int i = 0; i < H; i++){
		if(pipe(pipedFP[i])<0){	// apro H pipe grazie alla primitiva pipe
			printf("Errore nella creazione della pipe numero:%d\n",i);
			exit(2);
		}
	}

	for (int h = 0; h < H; h++)	// creo H processi figli, ognuno con indice h
	{		
		if ((pidFiglio = fork()) < 0)	/* Il processo padre crea un figlio */
		{	/* In questo caso la fork e' fallita */
			printf("Errore durante la fork\n");
			exit(3);
		}
		
		if (pidFiglio == 0)
		{	/* processo figlio */
			pipe_t pipedNF;		// pipe per la comunicazione tra nipoti e figli
			char ch;			// carattere usato per leggere il contenuto del file
			char store[250];    // array per la memorizzazione della linea passata dal nipote su pipe. Suppongo ogni linea possa contenere al massimo 250 caratteri compreso il terminatore di linea e di stringa
			int store_size = 0;	// lunghezza dell'array "store"
			int returnValue;	// variabile in cui memorizzo il valore di ritorno del processo figlio

			//chiudo tutte le pipe di pipedFP in lettura e scrittura tranne la h-esima pipe, aperta in scrittura
			for(int k = 0; k < H; k++){
				close(pipedFP[k][0]);
				if (k != h) {
					close(pipedFP[k][1]);
				}
			}
			
			if (pipe(pipedNF) < 0 )	// istanzio la pipe per la comunicazione tra nipoti e figli
			{	printf("Errore creazione pipe\n");
				exit(255);
			}
			
			if ((pidNipote = fork()) < 0)	/* Il processo figlio crea un nipote */
			{	/* In questo caso la fork e' fallita */
				printf("Figlio %d: Errore durante la fork\n", h);
				exit(255);
			}
			
			if (pidNipote == 0)
			{	/* processo nipote */

				close(pipedNF[0]);	// chiudo il lato di lettura della pipe tra nipoti e figli

				// chiudo lo stdout
				close(1);
				// duplico la pipe lato scrittura
				dup(pipedNF[1]);
				// chiudo il lato della pipe duplicato
				close(pipedNF[1]);
				
				execlp("rev", "rev", argv[h + 1], (char*)0);	// eseguo il comando rev sul file argv[h + 1]
				/* si esegue l'istruzione seguente SOLO in caso di fallimento della primitiva execlp */
				
				perror("errore esecuzione comando");
				exit(errno);
			}
			/* processo figlio */
			
			close(pipedNF[1]);	// chiudo il lato di scrittura della pipe tra nipoti e figli

			st.c3 = 0;	// inizializzo il campo c3 della struttura
			
			// suppongo ogni file termini con '\n'
			while (read(pipedNF[0], &ch, sizeof(char)) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
			{
				store[store_size++] = ch;	// inserisco il carattere letto in coda all'array store
				if (ch == '\n') {
					memcpy(st.c2, store, store_size);	// ho trovato il terminatore di linea, copio quindi la linea letta nella struttura da mandare al processo padre attraverso la funzione memcpy
					st.c3 = store_size;					// memorizzo la lunghezza della linea (compreso il terminatore di linea) sulla struttura
					store_size = 0;						// ripristino la lunghezza di store per le linee future
				}
			}

			if (st.c3 != 0) {
				// in questo caso il comando rev ha scritto qualcosa su stdout, quindi mando la struttura al processo padre dopo averne aggiornato il campo c1
				st.c1 = getpid();
				write(pipedFP[h][1], &st, sizeof(st));
				returnValue = st.c3 - 1;	// imposto il valore di ritorno su st.c3 - 1, la lunghezza dell'ultima linea letta tranne il terminatore di linea
			} else {
				// in questo caso il comando rev non ha scritto nulla su stdout, quindi mando al processo padre un valore di ritorno che indica errore, 255
				returnValue = 255;
			}

			// per correttezza, prima di chiudere il figlio faccio una wait del nipote
			if ((pidNipote = wait(&status)) < 0) {
				printf("Non e' stato creato nessun processo nipote\n");
				exit(255);
			}
			
			if ((status & 0xFF) != 0)
			{
				printf("Il processo nipote del figlio %d con PID %d e' stato terminato in modo anomalo\n", h, pidNipote);
			} else {
				ritorno = (status >> 8) & 0xFF;
				if (ritorno != 0) {	// stampo il valore di ritorno del processo nipote solo nel caso il valore di ritorno indichi un'errore nell'esecuzione del comando rev
					printf("Il processo nipote del figlio %d con PID %d ha ritornato un valore anomalo: %d\n", h, pidNipote, ritorno);
				}
			}

			exit(returnValue);
		}
		/* processo padre */
		
	}

	// chiudo tutte le pipe di pipedFP in scrittura
	for(int k = 0; k < H; k++){
		close(pipedFP[k][1]);
	}
	
	for (int h = 0; h < H; h++)	// ciclo di lettura delle pipe pipedFP
	{
		if (read(pipedFP[h][0], &st, sizeof(st)) == sizeof(st)) {	// controllo se e' stato scritto sul lato di lettura della pipe corrente il numero corretto di caratteri
			st.c2[st.c3] = 0;	// termino la stringa st.c2 mantenendo il terminatore della linea

			printf("Il padre ha ricevuto una struttura dal figlio relativo al file \"%s\"\n", argv[h + 1]);
			printf("\tPID=%d, lunghezza linea=%d, linea=\n\t%s\n", st.c1, st.c3, st.c2);
		} else {
			// in questo caso il figlio non ha scritto abbastanza byte su pipe o la pipe e' stata chiusa prima della scrittura (probabilmente il file corrispondente a questo figlio non esiste)
			printf("Il figlio di ordine %d non ha mandato alcuna struttura\n\n", h);
		}
	}
	
	for (int h = 0; h < H; h++)	// ciclo di attesa dei processi figli e stampa dei loro PID e valori di ritorno su schermo
	{
		if ((pidFiglio = wait(&status)) < 0) {
			printf("Non e' stato creato nessun processo figlio\n");
			exit(4);
		}
		
		if ((status & 0xFF) != 0)
		{
			printf("Il processo figlio con PID %d e' stato terminato in modo anomalo\n", pidFiglio);
		} else {
			ritorno = (status >> 8) & 0xFF;
			printf("Il figlio di PID %d ha ritornato %d\n",pidFiglio,ritorno);
		}
	}

	exit(0);	// uscita con successo
}
