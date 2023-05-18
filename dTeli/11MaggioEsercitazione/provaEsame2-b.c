#include <stdio.h>	// Includo la libreria per la funzione printf
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close, fork e exec
#include <fcntl.h>	// Includo la libreria per la funzione open e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <string.h>

struct {
    long int maxread;
    char Cx;
}dato;

int main(int argc, char** argv) {
    if (argc < 4) /* controllo sul numero di parametri: devono essere in numero maggiore a 3*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }
    //controllo se il file e' accedibile
    
    int piped[2];
    for(int i=0;i<argc-2;i++){
        if (strlen(argv[i+2]) != 1) {	// #include <string.h>
            printf("Errore, la stringa %s non e' un singolo carattere\n", argv[i]);
            exit(3);
        }
        //apro argc-2 pipes
        char C = argv[i+2][0];
        int pid;	// memorizzo il valore di ritorno della funzione fork
       if(pipe(piped)<0){
                printf("Errore nella creazione della pipe numero:%i",i);
        }
        if ((pid = fork()) < 0)	/* Il processo padre crea un figlio */
        {	/* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(4);
        }
        
        if (pid == 0)
        {	/* processo figlio */

            char N;
            close(piped[0]);
            int fd = 0; /*variabile che conterra il file descriptor del file che stiamo per aprire */
            if((fd = open(argv[1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
                printf("Errore in apertura file %s dato che fd = %d\n", argv[1], fd);
                exit(2);
            }
         
            dato.maxread=0;
            while (read(fd, &N, 1) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
            {
                if(N==C){
                    dato.maxread++;
                }
            }
            
            dato.Cx=C;
            int p=0;
            printf("DEBUG:maxread=%ld car=%c\n",dato.maxread,dato.Cx);
            if((p=write(piped[1],&dato,sizeof(dato)))<0){
                printf("ERRORE\n");
                exit(1);
            }
            printf("scrivo=%i\n",p);  
            
            exit(0);
        }
        
    }
    /* processo padre */
    //chiudo tutte le pipe in scrittura
    close(piped[1]);
    int pidchild;
    int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
    int ritorno;
    
    while (read(piped[0], &dato, sizeof(dato)) > 0){	/* in questo caso si e' usato un while fino a che ci sono informazioni da leggere sulla unica pipe */
    		printf("%ld occorrenze del carattere %c nel file %s\n",dato.maxread,dato.Cx, argv[1]);	/* i long int si stampano con %ld */
    }
  	/* Attesa della terminazione dei figli */
  	for(int i=0;i < argc-2;i++)
  	{
   		if ((pidchild = wait(&status)) < 0)
   		{
      			printf("Errore wait\n");
      			exit(5);
   		}
   		if ((status & 0xFF) != 0)
                	printf("Figlio con pid %d terminato in modo anomalo\n", pidchild);
   		else
   		{
                	ritorno=(int)((status >> 8) & 0xFF);
                	printf("Il figlio con pid=%d ha ritornato %c (%d se 255 problemi!)\n", pidchild, ritorno, ritorno); /* il padre stampa il valore ritornato come carattere; aggiunta la stampa come intero solo per intercettare un eventuale errore nel figlio */
   		}	
  	}
    exit(0);
}