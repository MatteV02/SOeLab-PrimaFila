/* FILE: pipe.c */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#define MSGSIZE 512

int main (int argc, char **argv)
{   
   int pid, j, piped[2]; 		/* pid per fork, j per indice, piped per pipe */
   char mess[MSGSIZE];			/* array usato dal figlio per inviare la stringa al padre e array usato dal padre per ricevere stringa inviata dal figlio */
   int pidFiglio, status, ritorno;      /* per wait padre */

	if (argc != 2)
	{	printf("Numero dei parametri errato %d: ci vuole un singolo parametro\n", argc);
    		exit(1);
	}
	/* si crea una pipe */
	if (pipe(piped) < 0 )  
	{   	printf("Errore creazione pipe\n");
    		exit(2); 
	}

	if ((pid = fork()) < 0)  
	{   	printf("Errore creazione figlio\n");
    		exit(3); 
	}
	if (pid == 0)  
	{   
		/* figlio */
		int fd;	/* variabile presente solo nello stack del figlio; va bene anche definirla nel main */
		close(piped[0]); 	/* il figlio CHIUDE il lato di lettura: sara' quindi lo SCRITTORE della pipe */
		if ((fd = open(argv[1], O_RDONLY)) < 0)		/* apriamo il file */
		{   	printf("Errore in apertura file %s\n", argv[1]);
            		exit(-1); /* torniamo al padre un -1 che sara' interpretato come 255 e quindi identificato come errore */
		}

		printf("DEBUG-Figlio %d sta per iniziare a scrivere una serie di messaggi, ognuno di lunghezza %d, sulla pipe dopo averli letti dal file passato come parametro\n", getpid(), MSGSIZE);
		j=0; /* il figlio inizializza la sua variabile j per contare i messaggi che mandera' al padre */
		/* il figlio legge tutto il file passato come parametro */
        int L=0;
        while (read(fd,&(mess[L]), 1)) /* il contenuto del file e' tale che in mess ci saranno 4 caratteri e il terminatore di linea */
		{   
                if(mess[L]=='\n'){
                    mess[L]='\0'; 
                    L++;        
    	            write(piped[1], mess, L);	/* il figlio che e' il produttore comunica al padre la linea letta dal file dopo averla trasformata in stringa (per il protocollo di comunicazione stabilito con il padre */
    			    L=0;
                    j++;
                    continue;
                }
                L++;
    			/* il padre ha concordato con il figlio che gli mandera' solo stringhe e quindi dobbiamo sostituire il terminatore di linea con il terminatore di stringa */
		}
        
		printf("DEBUG-Figlio %d scritto %d messaggi sulla pipe\n", getpid(), j);
		exit(j);	/* figlio deve tornare al padre il numero di linee lette che corrisponde al numero di stringhe mandate al padre */
	}

	/* padre */
    
	close(piped[1]); /* il padre CHIUDE il lato di scrittura: sara' quindi il LETTORE della pipe */	
    if ((pidFiglio=wait(&status)) < 0)
	{
      		printf("Errore wait\n");
      		exit(4);
	}
	if ((status & 0xFF) != 0)
        	printf("Figlio con pid %d terminato in modo anomalo\n", pidFiglio);
	else
	{
       		ritorno=(int)((status >> 8) & 0xFF);
       		printf("Il figlio con pid=%d ha ritornato %d (se 255 problemi!)\n", pidFiglio, ritorno);
	}	
	printf("DEBUG-Padre %d sta per iniziare a leggere i messaggi dalla pipe\n", getpid());
	j=0; /* il padre inizializza la sua variabile j per verificare quanti messaggi ha mandato il figlio */
	int lungezza=0;
    while (read(piped[0],&(mess[lungezza]),1))  /* questo ciclo avra' termine appena il figlio terminera' dato che la read senza piu' scrittore tornera' 0! */
	{ 
        	/* dato che il figlio gli ha inviato delle stringhe, il padre le puo' scrivere direttamente con una printf */
			
            if(mess[lungezza]=='\0'){
				printf("%d: %s\n",j, mess);
				j++;
                lungezza=-1;
			}
            lungezza++;

	}
	printf("DEBUG-Padre %d letto %d messaggi dalla pipe\n", getpid(), j);
	/* padre aspetta il figlio */

	exit(0);
}