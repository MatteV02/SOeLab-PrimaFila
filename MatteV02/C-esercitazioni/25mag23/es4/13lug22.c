#include <stdio.h>	// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe,
#include <fcntl.h>	// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <string.h>


//definisco il tipo pipe_t
typedef int pipe_t[2];

int main(int argc, char** argv) {
	int Q = argc - 3;   // memorizza il numero di caratteri passati
	char* Cx = malloc(Q * sizeof(*Cx)); // vettore di caratteri che memorizza i caratteri passati
	int L;		/*Dichiarazione varibile che conterra' argv[2]*/
	int q;	// dichiarazione contatore dei cicli for su Q
	int k;	// dichiarazione contatore dei cicli for di chiusura delle pipe
	int linea;	// variabile che scorre le linee del file
	int pidFiglio;	// memorizza il valore di ritorno della funzione fork e della funzione wait
	int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
	int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio

	if (argc < 4 + 1) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 4*/
	{
		printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
		exit(1);
	}



	//controllo utlizzando la funzione atoi se il argv[2] e un numero strettamente positivo
	if((L = atoi(argv[2])) <= 0){
		printf("il parametro %s non un numero positivo\n",argv[2]);
		exit(2);
	}

	for (q = 0; q < Q; q++)
	{
		//controlla che la stringa argv[q + 3] sia un singolo carattere
		if (strlen(argv[q + 3]) != 1) {	
			printf("Errore, la stringa %s non e' un singolo carattere\n", argv[q + 3]);
			exit(3);
		}
		Cx[q] = argv[q + 3][0];
	}
	

	//apro Q + 1 pipes
	pipe_t* piped = (pipe_t*)malloc(sizeof(pipe_t) * (Q + 1));
	if (piped == NULL) {
		printf("Errore nell'allocazione della memoria\n");
		exit(4);
	}
	
	for(q = 0; q < Q + 1; q++){
		if(pipe(piped[q])<0){
			printf("Errore nella creazione della pipe numero:%i\n",q);
			exit(4);
		}
	}

	for (q = 0; q < Q; q++)
	{	
		if ((pidFiglio = fork()) < 0)	/* Il processo padre crea un figlio */
		{	/* In questo caso la fork e' fallita */
			printf("Errore durante la fork\n");
			exit(5);
		}
		
		if (pidFiglio == 0)
		{	/* processo figlio */
			int fd = 0; /*variabile che conterra il file descriptor del file che stiamo per aprire */
			char c;	// variabile che memorizza i caratteri letti di volta in volta dal file
			int nOccorrenze;	// variabile che conta il numero di occorrenze del carattere Cx[q] nella linea considerata

			//chiudo tutte le pipe in lettura e scrittura tranne la (q + 1)-esima pipe, aperta in lettura e la (q + 2) % (Q + 1)-esima aperta il scrittura
			for(k = 0; k < Q + 1; k++){
				if (k != q) {
					close(piped[k + 1][0]);
					close(piped[(k + 2) % (Q + 1)][1]);
				}
			}

			//controllo se il file e' accedibile
			if((fd = open(argv[1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
				printf("Errore in apertura file %s dato che fd = %d\n", argv[1], fd);
				exit(255);
			}

			linea = 0;
			while (1) {	// continuo questo ciclo finché non viene letta l'ultima linea del file
				nOccorrenze = 0;	// preparo il contatore di occorrenze per la linea attuale
				while (read(fd, &c, sizeof(char)) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
				{
					//	se trovo un'occorrenza del carattere incremento il contatore
					if (c == Cx[q]) {
						nOccorrenze++;
					}
					
					// se incontro il terminatore di linea termino il conteggio delle occorrenze
					if (c == '\n') {
						break;
					}
				}

				// leggo dalla pipe di lettura del processo attuale l'indice della linea da cui sto leggendo (implementando lo schema di sincronizzazione a ring)
				if (read(piped[q + 1][0], &linea, sizeof(linea)) == 0) {
					printf("Errore, la pipe %d è stata chiusa prematuramente\n", q + 1); 
					exit(255);
				}	

				if (linea == L) {
					write(piped[(q + 2) % (Q + 1)][1], &linea, sizeof(linea));	// comunico al processo successivo che è stata raggiunta l'ultima linea del file
					break;
				}

				printf("Figlio con indice %d e pid %d ha letto %d caratteri %c nella linea corrente\n", q, pidFiglio, nOccorrenze, Cx[q]); 
				
				// scrivo al processo di ordine successivo il fatto che ho eseguito la stampa a schermo
				write(piped[(q + 2) % (Q + 1)][1], &linea, sizeof(linea));
			}

			exit(nOccorrenze);	// ritorno al processo padre il numero di occorrenze trovate nell'ultima linea del file
		}
		/* processo padre */
		
	}
	
	//chiudo tutte le pipe in lettura e scrittura tranne la 0-esima pipe, aperta in lettura, e la prima pipe aperta il scrittura
	for(k = 1; k < Q + 1; k++){
		close(piped[k][0]);
		close(piped[(k + 1) % (Q + 1)][1]);
	}

	for (linea = 0; linea < L; linea++)		// per ogni linea del file stampo in che linea mi trovo e comunico ai processi figli di procedere con conteggio delle occorrenze
	{
		printf("Linea %d del file %s\n", linea + 1, argv[1]); 
		write(piped[1][1], &linea, sizeof(linea));
		read(piped[0][0], &linea, sizeof(linea));	// attendo che l'ultimo processo del ring mandi il segnale di conclusione dell'operazione di conta delle occorrenze
	}

	write(piped[1][1], &L, sizeof(L));	// scrivo al figlio che ho letto l'ultima linea del file

	for (q = 0; q < Q; q++)	// for di attesa dei processi figli con stampa del valore di ritorno dei processi
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