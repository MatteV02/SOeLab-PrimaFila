#include <stdio.h>	// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe, dup
#include <fcntl.h>	// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <string.h>
//definisco il tipo pipe_t
typedef int pipe_t[2];

int main(int argc, char** argv) {
    if (argc < 1 + 1) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 1*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(-1);
    }
    //controlla che la stringa argv[argc-1] sia un singolo carattere
    if (strlen(argv[argc-1]) != 1) {
        printf("Errore, la stringa %s non e' un singolo carattere\n", argv[argc-1]);
        exit(-1);
    }
    char Cx = argv[argc-1][0];
    int N=argc-2;
    //apro N pipes
    pipe_t* pipedP = (pipe_t*)malloc(sizeof(pipe_t) * (N));
    if (pipedP == NULL) {
        printf("Errore nell'allocazione della memoria\n");
        exit(-1);
    }
    
    for(int i = 0;i < N;i++){
        if(pipe(pipedP[i])<0){
            printf("Errore nella creazione della pipe numero:%i\n",i);
            exit(-1);
        }
    }
    //apro N pipes
    pipe_t* pipedF = (pipe_t*)malloc(sizeof(pipe_t) * (N));
    if (pipedF == NULL) {
        printf("Errore nell'allocazione della memoria\n");
        exit(-1);
    }
    
    for(int i = 0;i < N;i++){
        if(pipe(pipedF[i])<0){
            printf("Errore nella creazione della pipe numero:%i\n",i);
            exit(-1);
        }
    }
    for(int i=0;i<N;i++){
        int pid;	// memorizzo il valore di ritorno della funzione fork
        
        if ((pid = fork()) < 0)	/* Il processo padre crea un figlio */
        {	/* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(-1);
        }
        
        if (pid == 0)
        {	/* processo figlio */
			//chiudo tutte le pipe in lettura e scrittura tranne la i-esima pipe, aperta in scrittura
			for(int k = 0; k < N; k++){
				close(pipedF[k][0]);
				close(pipedP[k][1]);
				if (k != i) {
					close(pipedF[k][1]);
					close(pipedP[k][0]);
				}
			}
            //controllo se il file e' accedibile
            int fd = 0; /*variabile che conterra il file descriptor del file che stiamo per aprire */
            if((fd = open(argv[i+1], O_RDWR)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
                printf("Errore in apertura file %s dato che fd = %d\n", argv[i+1], fd);
                exit(-1);
            }

            char buffer;
            int nsost=0;
            while (read(fd, &buffer, 1) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
            {
                if(buffer==Cx){
                    
                    write(pipedF[i][1],&buffer,1);
                    read(pipedP[i][0],&buffer,1);
                    
                    if(buffer!='\n'){
                        lseek(fd,-1L,SEEK_CUR);
                        write(fd,&buffer,1);
                        nsost++;
                    }

                }
			
			}

            exit(nsost);
        }
        
        
    }
    /* processo padre */
	for(int k = 0; k < N; k++){
		close(pipedF[k][1]);
		close(pipedP[k][0]);
	}
	int terminati;
	char buffer,scarto;
	while(!terminati){
		for(int i=0;i<N;i++){
			terminati=1;
			if((read(pipedF[i][0],&buffer,1)>0))
			{
				terminati=0;
				printf("trovato un valore %c inserire nuovo valore lasciare vuoto per non sostituire\n",Cx);
				read(0,&buffer,1);
				if(buffer!='\n'){read(0,&scarto,1);}
				write(pipedP[i][1],&buffer,1);
			}
		}
	}
	for(int i=0;i<N;i++){
		int pidFiglio;
		int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
		int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
		if ((pidFiglio = wait(&status)) < 0) {
			printf("Non e' stato creato nessun processo figlio\n");
			exit(-1);
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