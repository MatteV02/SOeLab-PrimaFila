#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    int nParametri = argc - 1;

    if (nParametri != 2) {
        printf("Errore, il numero di parametri passati dev'essere 2\n");
        exit(1);
    }

    int F = open(argv[1], O_RDONLY);
    if (F < 0) {
        printf("Errore, il primo parametro non è il percorso di un file leggibile\n");
        exit(2);
    }

    int n = atoi(argv[2]);
    if (n <= 0) {
        printf("Errore, il secondo parametro non rappresenta un numero strettamente positivo\n");
        exit(3);
    }

    int Flenght = lseek(F, 0, SEEK_END);
    lseek(F, 0, SEEK_SET);

    for(int i = 1; i * n < Flenght; i++) {
        lseek(F, i * n, SEEK_SET);
        char c;
        read(F, &c, sizeof(c));
        printf("Il carattere multiplo %d-esimo all'interno del file %s e' %c\n", i, argv[1], c);
    }

    exit(0);
}