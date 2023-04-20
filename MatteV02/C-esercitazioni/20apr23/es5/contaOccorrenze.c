#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

long int contaOccorrenze(int F, char Cx) {
    long int conta = 0;
    int caratteriEstratti;
    char carattere;

    while(1) {
        caratteriEstratti = read(F, &carattere, 1);
        //printf("Leggo il carattere %c", carattere);
        if (caratteriEstratti != 1) {
            break;
        }

        if (carattere == Cx) {
            conta++;
        }
    }

    return conta;
}

int main(int argc, char** argv) {
    int nparametri = argc - 1;

    if (nparametri != 2) {
        printf("Errore, devi passare 2 parametri\n");
        exit(1);
    }

    int F;
    char Cx;

    F = open(argv[1], O_RDONLY);
    if (F < 0) {
        printf("Errore, %s non corrisponde ad un nome di file\n", argv[1]);
        exit(2);
    }

    if (strlen(argv[2]) != 1) {
        printf("Errore, %s non è un singolo carattere\n", argv[2]);
    }
    Cx = argv[2][0];

    long int conta = contaOccorrenze(F, Cx);

    printf("Il file %s contiene %ld occorrenze del carattere %c\n", argv[1], conta, Cx);

    exit(0);
}