#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char **argv){
    /*controllo che siano passati esattamente due parametri*/
    if(argc!=3){
        printf("Errore: numero di parametri passati sbagliato dato che argc = %d\n", argc);
        exit(1);
    }
    //printf("Controllo parametri fatto\n");
    /*se arrivo qui il numero di parametri passati e' corretto*/
    /*controllo che il primo parametro sia un nome di file usando la primitiva open*/
    int fd = open(argv[1], O_RDONLY);
    /*se fd < 0 la open ha fallito*/
    if(fd < 0){
        printf("Errore nell'apertura del file\n");
        exit(2);
    }
    //printf("Apertura file fatta\n");
    /*controllo che il secondo parametro sia un numero intero > 0*/
    int n = atoi(argv[2]);
    /*se il numero e' minore o uguale a zero non va bene*/
    if(n <= 0){
        printf("Errore: %i non maggiore di 0\n", n);
        exit(3);
    }
    //printf("Conversione numero fatta\n");
    /*se arrivo qui i parametri sono corretti*/
    /*alloco dinamicamente un array di n caratteri che contiene i caratteri letti dal file*/
    char* buff = malloc(n * sizeof(*buff));
    //printf("DEBUG: faccio malloc\n");
    /*se la malloc fallisce*/
    if(buff == NULL){
        printf("Errore nella creazione del buffer");
        exit(4);
    }
    /*variabile che mi dice quanti caratteri ho letto*/
    int nread;
    /*variabile che mi dice a che multiplo sono*/
    int multiplo = 1;

    //printf("Comincio il ciclo\n");
    /*fino a quando leggo n caratteri*/
    while(1){
        //printf("prima della read\n");
        nread = read(fd, buff, n);

        //printf("Letti %d caratteri", nread);

        if (nread != n) {
            break;
        }

        printf("Il multiplo %d-esimo all'interno del file %s e' %c", multiplo, argv[1], buff[n-1]);
        multiplo++;
    }
    /*finita la stampa libero la memoria allocata dinamicamente*/
    free(buff);
    exit(0);
}