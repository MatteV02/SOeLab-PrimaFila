#include <stdio.h>	// Includo la libreria per la funzione printf
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close, fork e exec
#include <fcntl.h>	// Includo la libreria per la funzione open e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <ctype.h>
//definisco il tipo pipe_t
typedef int pipe_t[2];

int main(int argc, char** argv) {
    if (argc < 3) /* controllo sul numero di parametri: devono essere in numero maggiore a 2*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }
    int pidChild;	// memorizzo il valore di ritorno della funzione fork
    int N=argc-1;
    pipe_t piped[2];
    if (pipe(piped[0]) < 0 )
    {	printf("Errore creazione pipe\n");
        exit(5);
    }
     if (pipe(piped[1]) < 0 )
    {	printf("Errore creazione pipe\n");
        exit(5);
    }
    for(int i=1;i<=N;i++){

    int fd=0;
    if ((fd = open(argv[i], O_RDONLY)) < 0)		/* apriamo il file */
		{   	printf("Errore in apertura file %s\n", argv[i+1]);
            		exit(-1); /* torniamo al padre un -1 che sara' interpretato come 255 e quindi identificato come errore */
		}
    printf("DEBUG-Figlio %d sta per iniziare ad %i\n", getpid(),i);
    if ((pidChild = fork()) < 0)	/* Il processo padre crea un figlio */
    {	/* In questo caso la fork e' fallita */
        printf("Errore durante la fork\n");
        exit(2);
    }
    if (pidChild == 0)
    {	/* processo figlio */
        close(piped[1][0]);
        close(piped[0][0]);
        char car=0;
        if(i%2==0){
            close(piped[0][1]);
        }
        else{
            close(piped[1][1]);
        }
        while (read(fd,&car, 1)>0) /* il contenuto del file e' tale che in mess ci saranno 4 caratteri e il terminatore di linea */
		{
            printf("DEBUG:car=%c\n",car);
            if(i%2==0){
                if(isdigit(car)){
                    write(piped[1][1],&car,1);
                }
            }
            else{
                if(isalpha(car)){
                    write(piped[0][1],&car,1);
                }
            }
        }
        exit(car);
    }
    }
    /* processo padre */
    close(piped[1][1]);
    close(piped[0][1]);
    char carN=0,carD=0;
    int contatore=0,retD,retN;
    char * buffer=malloc(BUFSIZ*1);
    while(1){
        if((retN=read(piped[0][0],&carN,1))>0){
            buffer[contatore]=carN;
            contatore++;
        }
        if((retD=read(piped[1][0],&carD,1))>0){
            buffer[contatore]=carD;
            contatore++;
        }
        if(retD==0&&retN==0){
            break;
        }
    }
    printf("numero di caratteri=%i\n",contatore);
    buffer[contatore+1]='\0';
    printf("%s\n",buffer);
    for(int i=0;i<N;i++){
    int pidReturn;
    int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
    int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
    if ((pidReturn = wait(&status)) < 0) {
        printf("Non e' stato creato nessun processo figlio\n");
        exit(3);
    }
    
    if ((status & 0xFF) != 0)
    {
        printf("Il processo figlio è stato terminato in modo anomalo\n");
        exit(4);
    } else {
        ritorno = (status >> 8) & 0xFF;
    }
    printf("il figlio pid=%i ed ha ritornato=%i\n",pidReturn,ritorno);
    }
    exit(0);

}