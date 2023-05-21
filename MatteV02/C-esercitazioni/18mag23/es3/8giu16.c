#include <stdio.h>	// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close, fork, exec, read, write
#include <fcntl.h>	// Includo la libreria per la funzione open e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <time.h>   // per inizializzare il generatore di numeri casuali

int mia_random(int n)
{
    int casuale;
    casuale = rand() % n;
    return casuale;
}

//definisco il tipo pipe_t
typedef int pipe_t[2];

int main(int argc, char** argv) {
    int N = argc - 2; // memorizzo il numero dei file
    int H;		/*Dichiaro varibile che conterra' argv[argc - 1]*/
    int Fcreato; // file temporaneo
    int lunghezza_da_considerare;   // in questa variabile memorizzo quale fra tutte le lunghezze passate dai figli considerare
    int lunghezza_linea;    // in questa variabile memorizzo la lunghezza della linea
    int posizione_carattere;    // in questa variabile memorizzo la posizione del carattere da estrarre
    int pidFiglio;  // in questa variabile memorizzo il pid del processo figlio
    int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
    int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
    pipe_t* pipedPF, * pipedFP;  // dichiato 2 variabili per memorizzare le pipe per la comunicazione da padre a figli e da figli a padre
    
    if (argc < 5 + 1) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 5*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }

    //controllo utlizzando la funzione atoi se il argv[argc - 1] e un numero strettamente positivo
    if((H = atoi(argv[argc - 1])) <= 0 || H >= 255){
        printf("il parametro %s non un numero positivo o è maggiore o uguale a 255\n",argv[argc - 1]);
        exit(2);
    }

    srand(time(NULL));

    Fcreato = creat("/tmp/creato", 0644); // creo il file /tmp/creato con diritti di lettura e scrittura
    if (Fcreato < 0) {
        printf("Errore, errore in creazione del file temporaneo\n"); 
        exit(3);
    }

    //apro N pipes
    pipedPF = (pipe_t*)malloc(sizeof(pipe_t) * (N));
    if (pipedPF == NULL) {
        printf("Errore nell'allocazione della memoria\n");
        exit(4);
    }
    
    for(int i = 0;i < N;i++){
        if(pipe(pipedPF[i])<0){
            printf("Errore nella creazione della pipe numero:%i\n",i);
            exit(4);
        }
    }

    //apro N pipes
    pipedFP = (pipe_t*)malloc(sizeof(pipe_t) * (N));
    if (pipedFP == NULL) {
        printf("Errore nell'allocazione della memoria\n");
        exit(5);
    }
    
    for(int i = 0;i < N;i++){
        if(pipe(pipedFP[i])<0){
            printf("Errore nella creazione della pipe numero:%i\n",i);
            exit(5);
        }
    }

    for (int i = 0; i < N; i++)
    {        
        if ((pidFiglio = fork()) < 0)	/* Il processo padre crea un figlio */
        {	/* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(6);
        }
        
        if (pidFiglio == 0)
        {	/* processo figlio */
            //chiudo tutte le pipe in lettura e scrittura tranne le i-esime pipe
            for(int k = 0; k < N; k++){
                close(pipedPF[k][1]);
                close(pipedFP[k][0]);
                if (k != i) {
                    close(pipedPF[k][0]);
                    close(pipedFP[k][1]);
                }
            }

            char c; // in questa variabile memorizzo di volta in volta il carattere estratto dal file argv[i + 1]
            char linea[255];    // suppongo che la lunghezza massima di una linea sia 255 caratteri
            lunghezza_linea = 0;
            int caratteri_scritti = 0;  // in questa variabile memorizzo il numero di caratteri scritti su file, che ritornerò al padre
            
            //controllo se il file e' accedibile
            int fd = 0; /*variabile che conterra il file descriptor del file che stiamo per aprire */
            if((fd = open(argv[i + 1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
                printf("Errore in apertura file %s dato che fd = %d\n", argv[i + 1], fd);
                exit(254);
            }

            while (read(fd, &c, 1) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
            {
                linea[lunghezza_linea] = c;
                lunghezza_linea++;
            
                if (c == '\n') {
                    write(pipedFP[i][1], &lunghezza_linea, sizeof(lunghezza_linea));

                    if (read(pipedPF[i][0], &posizione_carattere, sizeof(posizione_carattere)) == 0) {
                        printf("Errore, pipe chiusa nel padre rispetto al figlio %d\n", i); 
                        exit(255);
                    }

                    printf("DEBUG:il figlio %d riceve dal padre la lunghezza %d\n", i, lunghezza_linea);

                    if (posizione_carattere > lunghezza_linea) {

                    } else {
                        if (write(Fcreato, linea + posizione_carattere, 1) == 0) {
                            printf("Errore in scrittura sul file temporaneo da parte del figlio %d\n", i); 
                            exit(254);
                        }

                        printf("DEBUG:scritto dal figlio %d sul file temporaneo il carattere %c\n", i, linea[posizione_carattere]);

                        caratteri_scritti++;
                    }

                    lunghezza_linea = 0;
                }
            }

            exit(caratteri_scritti);
        }
        /* processo padre */
    }

    //chiudo tutte le pipe in lettura e scrittura tranne la i-esima pipe, aperta in scrittura
    for(int k = 0; k < N; k++){
        close(pipedPF[k][0]);
        close(pipedFP[k][1]);
    }
    
    lunghezza_da_considerare = mia_random(N);

    for (int r = 0; r < H; r++) {
        for (int i = 0; i < N; i++)
        {
            if (read(pipedFP[i][0], &lunghezza_linea, sizeof(lunghezza_linea)) == 0) {
                printf("Errore, il lato di scrittura della pipe %d e' chiuso\n", i); 
                exit(6);
            }

            printf("DEBUG:Letto dal figlio %d la lunghezza %d\n", i, lunghezza_linea);

            if (i == lunghezza_da_considerare) {
                posizione_carattere = mia_random(lunghezza_linea);

                for (int k = 0; k < N; k++)
                {
                    printf("DEBUG:Il padre manda al figlio %d la posizione %d\n", i, posizione_carattere);
                    write(pipedPF[k][1], &posizione_carattere, sizeof(posizione_carattere));
                }
            }
        }
    }
    
    for (size_t i = 0; i < N; i++)
    {
        if ((pidFiglio = wait(&status)) < 0) {
            printf("Non e' stato creato nessun processo figlio\n");
            exit(7);
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