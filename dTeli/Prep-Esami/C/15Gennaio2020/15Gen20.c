/* Soluzione della Prova d'esame del 15 Gennaio 2020 - Parte C */
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <ctype.h>

typedef int pipe_t[2];

int main(int argc, char **argv) 
{
	/* -------- Variabili locali ---------- */
	int pid;			/* process identifier per le fork() */
	int N; 				/* numero di file passati sulla riga di comando */
	int status;			/* variabile di stato per la wait */
	pipe_t pipedFN;			/* pipe per comunicazione/sincronizzazione figlio-nipote: N.B. NON serve un array dinamico di pipe  */
	pipe_t pipedNF;			/* pipe per comunicazione/sincronizzazione nipote-figlio: N.B. NON serve un array dinamico di pipe  */
	int fd;				/* file descriptor che serve sia ai figli che ai nipoti */
	int primo=1;			/* serve al figlio per capire se è il primo iro! */
	char FOut[10];			/* nome file che devono creare i figli: allochiamo un array statico di 10 caratteri ipotizzando che bastino dato che il nome deve essere composto dalla strimga "merge" (5 caratteri) e dalla stringa corrispondente a i (supporta al massimo di 4 cifre) e quindi il terminatore di stringa ('\0')  */
	int fdOut;			/* file descriptor che serve ai figli e ai nipoti per scrivere sul file: N.B. File Pointer CONDIVISO! */
	int i;				/* indice per i cicli */
	char ch; 			/* variabile che serve per leggere i caratteri sia da parte dei figli che dei nipoti */
	char synch; 			/* variabile che serve per inviare, per la sincronizzazione, un carattere sia da parte dei figli che dei nipoti: NON IMPORTA IL SUO VALORE! */
	char ritorno; 			/* variabile che viene ritornata da ogni figlio al padre e da ogni nipote al figlio (SOLO PER MAGGIOR CHIAREZZA) */
	
	/* ------------------------------------ */
	
	/* Controllo sul numero di parametri: maggiore o uguale a due */
	if (argc < 3) /* Meno di due parametri */  
	{
		printf("Errore nel numero dei parametri %d\n", argc-1);
		exit(1);
	}

	/* Controllo sul numero di parametri: pari */
	if ((argc-1) % 2 != 0) /* dispari*/  
	{
		printf("Errore nel numero dei parametri %d\n", argc-1);
		exit(2);
	}

	/* Calcoliamo il numero di file passati */
	N = argc - 1;

	/* Ciclo di generazione dei figli: ATTENZIONE N/2 */
	for (i=0; i < N/2; i++)
	{
		if ( (pid = fork()) < 0)
		{
			printf("Errore nella fork %d-esima\n", i);
			exit(3);
		}
		
		if (pid == 0) 
		{
			/* codice del figlio */
			printf("Sono il processo figlio di indice %d e pid %d e leggero' dal file %s\n", i, getpid(), argv[i+1]);
			/* Creazione delle 2 pipe figlio-nipote e nipote-figlio: conviene creare le due pipe direttamente nel figlio dato che devono essere usato solo da figlio e dal nipote (figlio del figlio)! */
			if (pipe(pipedFN) < 0)
			{
				printf("Errore nella creazione della pipe %d-esima figlio-nipote\n", i);
				exit(-1); /* decidiamo di tornare -1 che verra' interpretato dal padre come 255 e quindi un valore non ammissibilei dato che il padre si aspetta un carattere! */
			}
			if (pipe(pipedNF) < 0)
			{
				printf("Errore nella creazione della pipe %d-esima nipote-figlio\n", i);
				exit(-1);
			}
		 
			/* il figlio deve creare il file, prima di creare il nipote */
			/* componiamo per prima cosa il nome del file da creare */
			sprintf(FOut, "merge%d", i);
			if ((fdOut=creat(FOut, 0644)) < 0)
		 	{
	                	printf("Errore nella creat del file %s\n", FOut);
         			exit(-1); 
                        }

			if ( (pid = fork()) < 0)
			{
				printf("Errore nella fork di creazione del nipote\n");
				exit(-1); 
			}	
			if (pid == 0) 
			{
				/* codice del nipote: attenzione all'indice da usare per associare ogni nipote al giusto file! */
				printf("Sono il processo nipote del figlio di indice %d e ho pid %d e leggero' dal file %s\n", i, getpid(), argv[N/2+i+1]);
				/* chiusura delle pipe non usate */
				close(pipedFN[1]);
				close(pipedNF[0]);

				/* il nipote deve aprire il suo file in sola lettura: nota bene il testo indica in modo esplicito l'indice da usare! */
				if ((fd=open(argv[N/2+i+1], O_RDONLY)) < 0)
				{
                                	printf("Errore nella open del file %s\n", argv[N/2+i+1]);
					exit(-1); /* anche per il nipote, decidiamo di tornare -1 che verra' interpretato dal padre (al quale viene passato dal figlio) come 255 e quindi un valore non ammissibile! */
                        	}
		        	while (read(fd, &ch, 1))
				{	
					/* controlliamo se dobbiamo scrivere aspettando l'indicazione dal figlio */
					read(pipedFN[0], &synch, 1);  
					/* NOTA BENE: non importa controllare che la lettura sia andata a buon fine dato che avendo i file la stessa lunghezza il processo figlio e nipote finiranno in pari! */
					write(fdOut, &ch, 1); /* scriviamo sul file */
					/* il nipote deve inviare al figlio che puo' scrivere a sua volta */
					write(pipedNF[1], &synch, sizeof(synch));
				}
				ritorno=ch; /* non strettamente necessario questo passaggio */	
				exit(ritorno); /* torniamo il valore richiesto dal testo */
			}
			else 
			{ /* codice figlio */
				/* chiusura delle pipe non usate */
				close(pipedFN[0]);
				close(pipedNF[1]);

				/* il figlio ha un codice molto simile al nipote */
				/* il figlio deve aprire il suo file in lettura: nota bene con il 'solito' indice! */
				if ((fd=open(argv[i+1], O_RDONLY)) < 0)
				{
                                	printf("Errore nella open del file %s\n", argv[i+1]);
					exit(-1); 
                        	}
		        	while (read(fd, &ch, 1))
				{	
					if (primo==1) /* si resetta questa variabile in modo che dopo la lettura di un carattere si vada sempre a ricevere l'OK dal nipote */
						primo=0;	
				        else 
						read(pipedNF[0], &synch, 1);
					write(fdOut, &ch, 1); /* scriviamo subito sul file */
					/* il figlio deve inviare al nipote che puo' scrivere a sua volta */
					write(pipedFN[1], &synch, sizeof(synch));
					/* controlliamo se dobbiamo leggere e scrivere aspettando l'indicazione dal nipote */
				}
				
				/* il figlio deve aspettare il nipote. Non e' richiesta la stampa la inseriamo lo stesso per controllo: stampiaamo il suo pid con il valore ritornato */
				pid = wait(&status);
				if (pid < 0)
				{	
					printf("Errore in wait\n");
					exit(-1);
				}
				if ((status & 0xFF) != 0)
    					printf("Nipote con pid %d terminato in modo anomalo\n", pid);
    				else
					/* ATTENZIONE: stampa non richiesta, ma nel caso si deve stampare il valore in termini di CARATTERE (come dovra' fare il padre)! */
					printf("Il nipote con pid=%d ha ritornato %c\n", pid, ritorno=(status >> 8) & 0xFF);

				/* torniamo il valore richiesto dal testo */
				exit(ritorno); 
			}
		}
	}
	
/* Codice del padre */

	/* Il padre aspetta i figli */
	for (i=0; i < N/2; i++)
	{
		pid = wait(&status);
		if (pid < 0)
		{
			printf("Errore in wait\n");
			exit(4);
		}

		if ((status & 0xFF) != 0)
    			printf("Figlio con pid %d terminato in modo anomalo\n", pid);
    		else
		{
			ritorno=(int)((status >> 8) &	0xFF); 
		  	if (ritorno==255)
 				printf("Il figlio con pid=%d ha ritornato %d e quindi vuole dire che ci sono stati dei problemi\n", pid, ritorno);
		  	else  printf("Il figlio con pid=%d ha ritornato %c\n", pid, ritorno);
			/* ATTENZIONE: si deve stampare il valore in termini di CARATTERE! */
		}
	}
	exit(0);
}
