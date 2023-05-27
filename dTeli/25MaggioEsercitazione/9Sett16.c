#include <stdio.h>	// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe, dup
#include <fcntl.h>	// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <stdbool.h>
//definisco il tipo pipe_t
typedef int pipe_t[2];
struct msg{
    char v1; //caratarattere
    long int v2; //occorrenze
};
void bubbleSort(struct msg v[], int dim)
{ 
    int i; bool ordinato = false;
    struct msg data;
    while (dim>1 && !ordinato)
    { 
        ordinato = true; /* hp: è ordinato */
        for (i=0; i<dim-1; i++){
            if (v[i].v2<v[i+1].v2)
            {
                data=v[i];
                v[i]=v[i+1];
                v[i+1]=data;
                ordinato = false;
            }
        }
    dim--;
    }
}
int main(int argc, char** argv) {
    int pidFiglio[26];	// memorizzo il valore di ritorno della funzione fork
    if (argc != 1 + 1) /* controllo sul numero di parametri: devono essere in numero uguale a 1*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(-1);
    }

    //apro 26 pipes
    pipe_t* piped = (pipe_t*)malloc(sizeof(pipe_t) * (26));
    if (piped == NULL) {
        printf("Errore nell'allocazione della memoria\n");
        exit(-1);
    }
    
    for(int i = 0;i < 26;i++){
        if(pipe(piped[i])<0){
            printf("Errore nella creazione della pipe numero:%i\n",i);
            exit(-1);
        }
    }
    struct msg output[26];
    for(int i=0;i<26;i++){
        
        if ((pidFiglio[i] = fork()) < 0)	/* Il processo padre crea un figlio */
        {	/* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(-1);
        }

        if (pidFiglio[i] == 0)
        {	/* processo figlio */
            //chiudo tutte le pipe in lettura e scrittura tranne la i-esima pipe, aperta in  lettura
            for(int k = 0; k < 26; k++){
                if(k!=i-1 && i!=0){
                    close(piped[k][0]); //chiudo tutte le pipe in lettura tranne i i-1-esima
                }
                if(!(k==i)){
                    close(piped[k][1]);   ///chiudo tutt le pipe in scrittura tranne le i-esima
                }
                if(i==0){
                    close(piped[i][0]);
                }
                
                                
            }
            if(i!=0){
                read(piped[i-1][0],&output,sizeof(output));
            }
            //controllo se il file e' accedibile
            int fd = 0; /*variabile che conterra il file descriptor del file che stiamo per aprire */
            if((fd = open(argv[1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
                printf("Errore in apertura file %s dato che fd = %d\n", argv[1], fd);
                exit(-1);
            }
            char lettera=97+i;
            char buffer;
            long int occorrenze=0;
            while (read(fd, &buffer, 1) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
            {
                if(buffer==lettera){
                    occorrenze++;
                }
            }
            output[i].v1=lettera;
            output[i].v2=occorrenze;

            write(piped[i][1],&output,sizeof(output));
            exit(buffer);
            
        }
      
        
    }
      /* processo padre */
      //chiudo tutte le pipe in lettura e scrittura tranne la i-esima pipe, aperta in  lettura
      for(int k = 0; k < 26; k++){
        close(piped[k][1]);
        if (k != 25) {
            close(piped[k][0]);
        }
      }
      
      read(piped[25][0],&output,sizeof(output));
      bubbleSort(output,26);

      for(int i=0;i<26;i++){
        printf("il figlio=%i ha trovato %li occorrenze del carattere %c \n",pidFiglio[i],output[i].v2,output[i].v1);
      }
      
      for(int i=0;i<26;i++){
        int pid;
        int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
        int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
        if ((pid = wait(&status)) < 0) {
            printf("Non e' stato creato nessun processo figlio\n");
            exit(-1);
        }
        
        if ((status & 0xFF) != 0)
        {
            printf("Il processo figlio è stato terminato in modo anomalo\n");
        } else {
            ritorno = (status >> 8) & 0xFF;
            printf("il figlio pid=%i ed ha ritornato=%i\n",pid,ritorno);
        }
      }


    exit(0);
}