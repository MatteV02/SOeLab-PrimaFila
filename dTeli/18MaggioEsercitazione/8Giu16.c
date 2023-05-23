#include <stdio.h>	// Includo la libreria per la funzione printf
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close, fork e exec
#include <fcntl.h>	// Includo la libreria per la funzione open e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <time.h>


//definisco il tipo pipe_t
typedef int pipe_t[2];
#define PERM 0777;

int mia_random(int n)
{
int casuale;
casuale = rand() % n;
return casuale;
}

int main(int argc, char** argv) {
    if (argc < 5) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 4*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }
    int H;		/*Inizializzo varibile che conterra' argv[argc-1]*/
    //controllo utlizzando la funzione atoi se il argv[argc-1] e un numero strettamente positivo
    if((H=atoi(argv[argc-1])) <= 0){
        printf("il parametro %s non un numero positivo\n",argv[argc-1]);
        exit(2);
    }
    srand(time(NULL));

    //apro argc-2 pipes
    pipe_t* pipedP=malloc(sizeof(pipe_t)*argc-2);
    for(int i=0;i<argc-2;i++){
        if(pipe(pipedP[i])<0){
            printf("Errore nella creazione della pipe numero:%i",i);
        }
    }

    //apro argc-2 pipes
    pipe_t* pipedF=malloc(sizeof(pipe_t)*argc-2);
    for(int i=0;i<argc-2;i++){
        if(pipe(pipedF[i])<0){
            printf("Errore nella creazione della pipe numero:%i",i);
        }
    }

    //controllo se il file e' accedibile
    //char filename[13]="/temp/creato";
    int fd = 0; /*variabile che conterra il file descriptor del file che stiamo per aprire */
    if((fd = creat("creato", 0640)) < 0){		/* ERRORE se non si riesce a creare il file */
        printf("Errore in creazione del file %s dato che fd = %d\n", "/temp/creato", fd);
        exit(2);
    }

    for(int i=0;i<argc-2;i++){
        int pidChild;	// memorizzo il valore di ritorno della funzione fork
        
        if ((pidChild = fork()) < 0)	/* Il processo padre crea un figlio */
        {	/* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(3);
        }
        
        if (pidChild == 0)
        {	/* processo figlio */

            //chiudo tutte le pipe in lettura
            for(int k=0;k<argc-2;k++){
                close(pipedF[k][0]);
                close(pipedP[k][1]);
                if(k!=i){
                    close(pipedF[k][1]);
                    close(pipedP[k][0]);
                }
            }
            
            //controllo se il file e' accedibile
            int fdd = 0; /*variabile che conterra il file descriptor del file che stiamo per aprire */
            if((fdd = open(argv[i+1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
                printf("Errore in apertura file %s dato che fd = %d\n", argv[i+1], fd);
                exit(4);
            }
            char buffer[255];
            int contatore=0;
            int indice=0;
            int scritti=0;
            while (read(fdd, &buffer[contatore], 1) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
            {
                contatore++;
                if(buffer[contatore-1]=='\n'){
                    write(pipedF[i][1],&contatore,sizeof(int));
                    read(pipedP[i][0],&indice,sizeof(indice));
                    if(indice<=contatore){
                        write(fd,&buffer[indice],1);
                        scritti++;
                    }
                    contatore=0;
                }
            }
            exit(scritti);
        }
        
        
    }
    /* processo padre */
    for(int k=0;k<argc-2;k++){
        close(pipedF[k][1]);
        close(pipedP[k][0]);
    }
    int* lungezze=malloc((argc-2)*sizeof(int));
    
    for(int j=0;j<H;j++){
    
    for(int i=0;i<argc-2;i++){
        read(pipedF[i][0],&lungezze[i],sizeof(int));
    }
    int randomLen = mia_random(argc-2);
    printf("DEBUG:random+%i\n",randomLen);
    int indice = mia_random(lungezze[randomLen]);
    for(int i=0;i<argc-2;i++){
        write(pipedP[i][1],&indice,sizeof(int));   
    }

    }

    for(int i =0;i<argc-2;i++){
        int pid;
        int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
        int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
        if ((pid = wait(&status)) < 0) {
            printf("Non e' stato creato nessun processo figlio\n");
            exit(5);
        }
        
        if ((status & 0xFF) != 0)
        {
            printf("Il processo figlio è stato terminato in modo anomalo\n");
            exit(5);
        } else {
            ritorno = (status >> 8) & 0xFF;
        }
        printf("il figlio pid=%i ed ha ritornato=%i\n",pid,ritorno);
    }


    exit(0);
}