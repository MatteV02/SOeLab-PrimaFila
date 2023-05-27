#include <stdio.h>	// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe, dup
#include <fcntl.h>	// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <ctype.h>
//definisco il tipo pipe_t
typedef int pipe_t[2];

int main(int argc, char** argv) {
    int N=argc-1;
    if (argc < 2 + 1) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 2*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(-1);
    }
    //apro argc-1 pipes
    pipe_t* pipedF = (pipe_t*)malloc(sizeof(pipe_t) * (argc-1));
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
        pipe_t* pipedN = (pipe_t*)malloc(sizeof(pipe_t) * (argc-1));
    if (pipedN == NULL) {
        printf("Errore nell'allocazione della memoria\n");
        exit(-1);
    }
    
    for(int i = 0;i < N;i++){
        if(pipe(pipedN[i])<0){
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
            int pidNipote;	// memorizzo il valore di ritorno della funzione fork
            
            if ((pidNipote = fork()) < 0)	/* Il processo padre crea un figlio */
            {	/* In questo caso la fork e' fallita */
                printf("Errore durante la fork\n");
                exit(-1);
            }
            
            if (pidNipote == 0)
            {	/* processo nipote */
                //chiudo tutte le pipe in lettura e scrittura tranne la i-esima pipe, aperta in scrittura
                for(int k = 0; k < N; k++){
                    close(pipedN[k][0]);
                    close(pipedF[k][0]);
                    close(pipedF[k][1]);
                    if (k != i) {
                        close(pipedN[k][1]);
                    }
                }
                //controllo se il file e' accedibile
                int fd = 0; /*variabile che conterra il file descriptor del file che stiamo per aprire */
                if((fd = open(argv[i+1], O_RDWR)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
                    printf("Errore in apertura file %s dato che fd = %d\n", argv[i+1], fd);
                    exit(-1);
                }
    
                char buffer;
                long int trasfomazioni=0;
                while (read(fd, &buffer, 1) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
                {
                    if(islower(buffer)){
                        lseek(fd,-1L,SEEK_CUR);
                        buffer=toupper(buffer);
                        
                        write(fd,&buffer,1);
                        trasfomazioni++;
                    }
                }
                write(pipedN[i][1],&trasfomazioni,sizeof(trasfomazioni));

            if(trasfomazioni<256){
                exit(0);
             }   
             if(trasfomazioni<512){
                exit(1);
             }

             if(trasfomazioni<768){
                exit(2);
             }
             exit(3);

            }
            /* processo figlio */  
                //chiudo tutte le pipe in lettura e scrittura tranne la i-esima pipe, aperta in scrittura
                for(int k = 0; k < N; k++){
                    close(pipedF[k][0]);
                    close(pipedN[k][0]);
                    close(pipedN[k][1]);
                    if (k != i) {
                        close(pipedF[k][1]);
                    }
                }  
                //controllo se il file e' accedibile
                int fd = 0; /*variabile che conterra il file descriptor del file che stiamo per aprire */
                if((fd = open(argv[i+1], O_RDWR)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
                    printf("Errore in apertura file %s dato che fd = %d\n", argv[i+1], fd);
                    exit(-1);
                }

                char buffer;
                long int trasfomazioni=0;
                char spazio=' ';
                while (read(fd, &buffer, 1) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
                {
                    if(isdigit(buffer)){
                        lseek(fd,-1L,SEEK_CUR);
                        write(fd,&spazio,1);
                        trasfomazioni++;
                    }
                }
                write(pipedF[i][1],&trasfomazioni,sizeof(trasfomazioni));
                int pidNip;
                int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
                int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
                if ((pidNip = wait(&status)) < 0) {
                    printf("Non e' stato creato nessun processo figlio\n");
                    exit(-1);
                }
                
                if ((status & 0xFF) != 0)
                {
                    printf("Il processo figlio è stato terminato in modo anomalo\n");
                } else {
                    ritorno = (status >> 8) & 0xFF;
                    printf("il nipote pid=%i ed ha ritornato=%i\n",pidNip,ritorno);
                } 

              
             if(trasfomazioni<256){
                exit(0);
             }   
             if(trasfomazioni<512){
                exit(1);
             }

             if(trasfomazioni<768){
                exit(2);
             }
             exit(3);
        }
        
        
        
    }
    /* processo padre */
    //chiudo tutte le pipe in scrittura
    for(int k = 0; k < N; k++){
        close(pipedN[k][1]);
        close(pipedF[k][1]);
    }
    long int tras=0;
    for(int i=0;i<N;i++){
        read(pipedF[i][0],&tras,sizeof(tras));
        printf("il file=%s il figlio ha ritornato=%li\n",argv[i+1],tras);
        read(pipedN[i][0],&tras,sizeof(tras));
        printf("il file=%s il nipote ha ritornato=%li\n",argv[i+1],tras);
    }
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
    exit(0);
}