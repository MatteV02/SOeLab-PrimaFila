#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>

int main(int argc, char **argv)
{
    int nParams = argc - 1;

    if (nParams != 2) {
        printf("Errore, devi passare 2 parametri\n");
        exit(1);
    }

    int F = open(argv[1], O_RDONLY);
    if (F < 0) {
        printf("Errore, il primo parametro non corrisponde ad un file leggibile!\n");
        exit(2);
    }

    int n = atoi(argv[2]);
    if (n <= 0) {
        printf("Errore, il secondo parametro non corrisponde ad un numero strettamente positivo\n");
        exit(3);
    }

    char linea[256] = "";

    bool trovata = false;

    int j = 0, conta = 1;

    while(read(F, &(linea[j]), 1) != 0) {
        if (linea[j] == '\n') {
            conta++;
            if (conta > n) {
                trovata = true;
                linea[j + 1] = 0;
                break;
            }
            j = 0;
        } else {
            j++;
        }
    }

    if (trovata) {
        printf("La linea %d-esima del file e': %s\n", n, linea);
    } else {
        printf("Errore: il file contiene %d linee, non %d\n", conta, n);
        exit(4);
    }

    exit(0);
}