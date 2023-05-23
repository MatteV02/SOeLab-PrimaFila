#include <stdio.h>	// Includo la libreria per la funzione printf
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close, fork e exec
#include <fcntl.h>	// Includo la libreria per la funzione open e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
//definisco il tipo pipe_t
typedef int pipe_t[2];

int main(int argc, char** argv) {
    if (argc < 3) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 2*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(2);
    }
    pipe_t* piped=malloc((argc-1)*sizeof(pipe_t));
    for(int i=0;i<argc-1;i++){
        
        
        if (pipe(piped[i]) < 0 )
        {	printf("Errore creazione pipe\n");
            exit(1);
        }

        int pidFiglio;	// memorizzo il valore di ritorno della funzione fork
        
        if ((pidFiglio = fork()) < 0)	/* Il processo padre crea un figlio */
        {	/* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(2);
        }
        
        if (pidFiglio == 0)
        {	/* processo figlio */
            //controllo se il file e' accedibile
            //apro argv-1 pipes
            //chiudo tutte le pipe in lettura
            for(int k=0;k<argc-1;k++){
                close(piped[k][0]);
            }
            //chiudo tutte le pipe in scrittura
            for(int k=0;k<argc-1;k++){
                if(k!=i){
                    close(piped[k][1]);
                }
            }
            int fd = 0; /*variabile che conterra il file descriptor del file che stiamo per aprire */
            close(0);
            if((fd = open(argv[i+1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
                printf("Errore in apertura file %s dato che fd = %d\n", argv[i+1], fd);
                exit(3);
            }
            int pidNipote;	// memorizzo il valore di ritorno della funzione fork
            
            int figliopipe[2];
            pipe(figliopipe);
            if ((pidNipote = fork()) < 0)	/* Il processo padre crea un figlio */
            {	/* In questo caso la fork e' fallita */
                printf("Errore durante la fork\n");
                exit(4);
            } 

            if (pidNipote == 0)
            {	/* processo nipote */
                close(piped[i][1]);
                close(figliopipe[0]);
                //chiudo lo stdin
                close(1);
                //duplico la pipe lato scrittura
                dup(figliopipe[1]);
                //chiudo la pipe duplicata
                close(figliopipe[1]);
                execlp("wc","wc","-l",0);
                exit(-1);
            }
            /* processo figlio */
            close(figliopipe[1]);
            char ritorno=0;
            char* buffer=malloc(BUFSIZ);
            int contatore=0;
            while(read(figliopipe[0],&ritorno,sizeof(ritorno))>0){
                buffer[contatore]=ritorno;
                contatore++;
            }
            buffer[contatore]=0;
            int lung=atoi(buffer);
            write(piped[i][1],&lung,sizeof(lung));
            int pidRitornoNip;
            int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
            int ritornoN;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
            if ((pidRitornoNip = wait(&status)) < 0) {
                printf("Non e' stato creato nessun processo figlio\n");
                exit(5);
            }
            
            if ((status & 0xFF) != 0)
            {
                printf("Il processo figlio è stato terminato in modo anomalo\n");
                exit(5);
            } else {
                ritornoN = (status >> 8) & 0xFF;
            }
            
            exit(ritornoN);
        }
    }
        /* processo padre */
        for(int i=0;i<argc-1;i++){
        int pidChild;
        int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
        int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
        if ((pidChild = wait(&status)) < 0) {
            printf("Non e' stato creato nessun processo figlio\n");
            exit(6);
        }
        
        if ((status & 0xFF) != 0)
        {
            printf("Il processo figlio è stato terminato in modo anomalo\n");
            exit(6);
        } else {
            ritorno = (status >> 8) & 0xFF;
        }
        printf("il figlio pid=%i ed ha ritornato=%i\n",pidChild,ritorno);
        }
        //chiudo tutte le pipe in scrittura
        for(int k=0;k<argc-1;k++){
            close(piped[k][1]);
        }

        int lungezzaRitorno=0,lungezzaTot=0;
        
        for(int i=0;i<argc-1;i++){
            read(piped[i][0],&lungezzaRitorno,sizeof(int));
            lungezzaTot+=lungezzaRitorno;
        }
        printf("la lunghezza totale di tutti i file %d\n",lungezzaTot);

    exit(0);
}