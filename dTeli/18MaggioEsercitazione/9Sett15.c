#include <stdio.h>	// Includo la libreria per la funzione printf
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close, fork e exec
#include <fcntl.h>	// Includo la libreria per la funzione open e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wa
#include <signal.h>
#include <stdbool.h>
//definisco il tipo pipe_t
typedef int pipe_t[2];

int main(int argc, char** argv) {
    bool continua=true;
    if (argc <= 4) /* controllo sul numero di parametri: devono essere in numero maggiore a 3*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(2);
    } 
    //apro argc-2 pipes
    pipe_t* pipedF=malloc(sizeof(pipe_t)*argc-2);   
    for(int i=0;i<argc-2;i++){
        if(pipe(pipedF[i])<0){
            printf("Errore nella creazione della pipe numero:%i",i);
        }
    }

    pipe_t* pipedP=malloc(sizeof(pipe_t)*argc-2);
    for(int i=0;i<argc-2;i++){
        if(pipe(pipedP[i])<0){
            printf("Errore nella creazione della pipe numero:%i",i);
        }
    }

    int *to_be_killed=calloc((argc-2),sizeof(int));
    int *pids=malloc(sizeof(int)*(argc-2));
    for(int i=0;i<argc-2;i++){
        if ((pids[i] = fork()) < 0)	/* Il processo padre crea un figlio */
        {	/* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(33);
        }
        
        if (pids[i] == 0)
        {	/* processo figlio */
            //chiudo tutte le pipe in lettura
            for(int k=0;k<argc-2;k++){
                close(pipedF[k][0]);
                close(pipedP[k][1]);
                if(i!=k){
                    close(pipedF[k][1]);
                    close(pipedP[k][0]);
                }
            }
            //controllo se il file e' accedibile
            int fd = 0; /*variabile che conterra il file descriptor del file che stiamo per aprire */
            if((fd = open(argv[i+1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
                printf("Errore in apertura file %s dato che fd = %d\n", argv[i+1], fd);
                exit(2);
            }
            char buffer;
            while (1)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
            {
                read(pipedP[i][0],&continua,1);
                if(read(fd, &buffer, 1)<=0){
                    break;
                }
                
                write(pipedF[i][1],&buffer,1);
            }
            exit(0);
        }

    }
    /* processo padre */
    //chiudo tutte le pipe in scrittura
    for(int k=0;k<argc-2;k++){
        close(pipedF[k][1]);
        close(pipedP[k][0]);
    }
    //controllo se il file e' accedibile
    int fd = 0; /*variabile che conterra il file descriptor del file che stiamo per aprire */
    if((fd = open(argv[argc-1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
        printf("Errore in apertura file %s dato che fd = %d\n", argv[argc-1], fd);
        exit(5);
    }
        char char_ctrl;
        while (1)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
        {
            for(int i=0;i<argc-2;i++){

                if(to_be_killed[i]!=1){
                    write(pipedP[i][1],&continua,1);
                }
            }
            if(read(fd, &char_ctrl, 1)<=0){
                break;
            }

            for(int i=0;i<argc-2;i++){
                char buffer;
                if(to_be_killed[i]!=1){
                read(pipedF[i][0], &buffer, 1);
                if(buffer!=char_ctrl){
                    to_be_killed[i]=1;
                }
                }
            }
        }
    for(int i=0;i<argc-2;i++){
        if(to_be_killed[i]==1){
                kill(pids[i],SIGKILL);   
            }
    }
    for(int i=0;i<argc-2;i++){
        int pidChild;
        int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
        int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
        if ((pidChild = wait(&status)) < 0) {
            printf("Non e' stato creato nessun processo figlio\n");
            exit(1);
        }
        
        if ((status & 0xFF) != 0)
        {
            printf("Il processo figlio=%i è stato terminato in modo anomalo\n",pidChild);
        } else {
            ritorno = (status >> 8) & 0xFF;
            printf("Il processo figlio=%i è stato terminato in modo normale\n",pidChild);
            printf("ha ritornato=%i",ritorno);
            for(int i=0;i<argc-2;i++){
                if(pids[i]==pidChild){
                    printf(" il file %s e uguale al file %s\n",argv[i+1],argv[argc-1]);
                }
            }
        
        }
    }
    


    exit(0);
}