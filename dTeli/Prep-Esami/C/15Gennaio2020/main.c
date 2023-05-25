#include <stdio.h>	// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe, dup
#include <fcntl.h>	// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
//definisco il tipo pipe_t
typedef int pipe_t[2];

int main(int argc, char** argv) {
    int N=(argc-1)/2; // numero di fasi
    int pidFiglio;
    int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
    int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
    int fdOut; // file descriptor per il file OUT
    char Fout[8];
    char ok;
    int pidNipote;	// memorizzo il valore di ritorno della funzione fork
    //int fd = 0; /*variabile che conterra il file descriptor del file che stiamo per aprire */
    char buffer;
    char start=0;
    if (argc < 2 + 1) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 2*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(-1);
    }
    
    for(int i=0;i<N;i++){
        
        
        if ((pidFiglio = fork()) < 0)	/* Il processo padre crea un figlio */
        {	/* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(-1);
        }
        
        if (pidFiglio == 0)
        {	/* processo figlio */
            sprintf(Fout,"merge%i",i);
            fdOut=creat(Fout,0644);
            pipe_t piped[2];
            for(int i=0;i<2;i++){
            if (pipe(piped[i]) < 0 )
            {	printf("Errore creazione pipe\n");
                exit(-1);
            }
            }
            if ((pidNipote = fork()) < 0)	/* Il processo figlio crea un nipote */
            {	/* In questo caso la fork e' fallita */
                printf("Errore durante la fork\n");
                exit(-1);
            }
            
            if (pidNipote == 0)
            {	/* processo Nipote */
                close (piped[0][1]);
                close (piped[1][0]);
                //controllo se il file e' accedibile
                int fd=0;
                if((fd = open(argv[i+2], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
                    printf("Errore in apertura file %s dato che fd = %d\n", argv[i+2], fd);
                    exit(-1);
                }
                int nread=0;
                while ((nread = read(fd, &buffer, 1)) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
                {
                    read(piped[0][0],&ok,sizeof(ok));   
                    write(fdOut,&buffer,sizeof(buffer));
                    write(piped[1][1],&ok,sizeof(ok));
                }
                exit(buffer);
            }
            /* processo figlio */
            close (piped[0][0]);
            close (piped[1][1]);
            int fd=0;
            if((fd = open(argv[i+1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
                printf("Errore in apertura file %s dato che fd = %d\n", argv[i+1], fd);
                exit(-1);
            }

            while (read(fd, &buffer, 1) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
            {
                if(start==0){
                    start=1;
                }else{
                    read(piped[1][0],&ok,sizeof(ok));
                }
                write(fdOut,&buffer,sizeof(buffer));
                write(piped[0][1],&ok,sizeof(ok));
            }
            
            if ((pidNipote = wait(&status)) < 0) {
                printf("Non e' stato creato nessun processo figlio\n");
                exit(-1);
            }
            
            if ((status & 0xFF) != 0)
            {
                printf("Il processo figlio è stato terminato in modo anomalo\n");
            } else {
                ritorno = (status >> 8) & 0xFF;
            
            }
            exit(buffer);
        }
        
        
    }
    /* processo padre */
    for(int i=0;i<N;i++){
        if ((pidFiglio = wait(&status)) < 0) {
            printf("Non e' stato creato nessun processo figlio\n");
            exit(-1);
        }

        for(int i=0;i<N;i++){
        
        if ((status & 0xFF) != 0)
        {
            printf("Il processo figlio è stato terminato in modo anomalo\n");
        } else {
            ritorno = (status >> 8) & 0xFF;
            printf("il figlio pid=%i ed ha ritornato=%i\n",pidFiglio,ritorno);
        }
        }
    }

    exit(0);
}