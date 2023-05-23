#include <stdio.h>	// Includo la libreria per la funzione printf
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close, fork e exec
#include <fcntl.h>	// Includo la libreria per la funzione open e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
//definisco il tipo pipe_t
typedef int pipe_t[2];
struct dati{
int c1;
int c2;
char c3[250];
}dato;

int main(int argc, char** argv) {
    if (argc < 4) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 3*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }
    //apro argc-1 pipes
    pipe_t* piped=malloc(sizeof(pipe_t)*argc-1);
    for(int i=0;i<argc-1;i++){
        if(pipe(piped[i])<0){
            printf("Errore nella creazione della pipe numero:%i",i);
        }
    }
    for(int i=0;i<argc-1;i++){
        int pid;	// memorizzo il valore di ritorno della funzione fork
        
        if ((pid = fork()) < 0)	/* Il processo padre crea un figlio */
        {	/* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(2);
        }
        
        if (pid == 0)
        {
            //chiudo tutte le pipe in lettura
            for(int k=0;k<argc-1;k++){
                close(piped[k][0]);
                if(k!=i){
                    close(piped[k][1]);
                }

            }
            //controllo se il file e' accedibile
            int pipefiglio[2];
            
            if (pipe (pipefiglio) < 0 )
            {	printf("Errore creazione pipe\n");
                exit(6);
            }
            int pidNeph;	// memorizzo il valore di ritorno della funzione fork
            
            if ((pidNeph = fork()) < 0)	/* Il processo padre crea un figlio */
            {	/* In questo caso la fork e' fallita */
                printf("Errore durante la fork\n");
                exit(7);
            }
            
            if (pidNeph == 0)
            {	/* processo nipote */
                close(0);
                int fd = 0; /*variabile che conterra il file descriptor del file che stiamo per aprire */
                if((fd = open(argv[i+1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
                    printf("Errore in apertura file %s dato che fd = %d\n", argv[i+1], fd);
                    exit(4);
                }
                close(pipefiglio[0]);
                //chiudo lo stdin
                close(1);
                //duplico la pipe lato scrittura
                dup(pipefiglio[1]);
                //chiudo la pipe duplicata
                close(pipefiglio[1]);
                execlp("sort","sort","-f",0);
                exit(-1);
            }
            /* processo figlio */
            close(pipefiglio[1]);
            char buffer;
            int j=0;
            while (read(pipefiglio[0], &buffer, 1) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
            {
                if(buffer=='\n'){
                    break;
                }
                dato.c3[j]=buffer;
                j++;
            }
            int pidN;
            int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
            if ((pidN = wait(&status)) < 0) {
                printf("Non e' stato creato nessun processo figlio\n");
                exit(7);
            }
            
            if ((status & 0xFF) != 0)
            {
                printf("Il processo figlio è stato terminato in modo anomalo\n");
                exit(7);
            }
            dato.c1=pidN;
            dato.c2=j;
            //printf("il file %s ha c1=%i c2=%i c3=%s\n",argv[i+1],dato.c1,dato.c2,dato.c3);
            write(piped[i][1],&dato,sizeof(dato));
            exit(dato.c2);
        }
    }
        /* processo padre */
        //chiudo tutte le pipe in scrittura
        for(int k=0;k<argc-1;k++){
            close(piped[k][1]);
        }
        for(int i=0;i<argc-1;i++){
            read(piped[i][0],&dato,sizeof(dato));
            dato.c3[dato.c2+1]=0;
            printf("il file %s ha c1=%i c2=%i c3=%s\n",argv[i+1],dato.c1,dato.c2,dato.c3);
        }
        for(int i=0;i<argc-1;i++){
        int pidChild;
        int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
        int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
        if ((pidChild = wait(&status)) < 0) {
            printf("Non e' stato creato nessun processo figlio\n");
            exit(8);
        }
        
        if ((status & 0xFF) != 0)
        {
            printf("Il processo figlio è stato terminato in modo anomalo\n");
            exit(8);
        } else {
            ritorno = (status >> 8) & 0xFF;
        }
        printf("il figlio pid=%i ed ha ritornato=%i\n",pidChild,ritorno);
        }

    exit(0);
}