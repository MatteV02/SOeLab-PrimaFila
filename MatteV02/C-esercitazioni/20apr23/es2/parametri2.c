#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char** argv)
{
    int nparametri = argc - 1;

    if (nparametri != 3) {
        printf("Errore, devi passare 3 parametri\n");
        exit(1);
    }

    int file;
    int N;
    char C;

    file = open(argv[1], O_RDONLY);
    if (file < 0) {
        printf("Errore, %s non corrisponde ad un nome di file\n", argv[1]);
        exit(2);
    }
    close(file);

    N = atoi(argv[2]);
    if (N < 0) {
        printf("Errore, %s non e' un numero strettamente positivo\n", argv[2]);
        exit(3);
    }

    if (strlen(argv[3]) != 1) {
        printf("Errore, %s non e' un singolo carattere\n", argv[3]);
        exit(4);
    }
    C = argv[3][0];

    printf("L'eseguibile si chiama: %s\n", argv[0]);
    printf("Sono stati passati %d parametri\n", nparametri);
    printf("Il primo parametro è un nome di file %s", argv[1]);
    printf("Il secondo parametro è un numero intero strettamente positivo: %d\n", N);
    printf("Il terzo parametro e' un carattere singolo: %c\n", C);

    exit(0);
}