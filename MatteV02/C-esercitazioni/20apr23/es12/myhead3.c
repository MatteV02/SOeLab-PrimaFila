#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    int nParametri = argc - 1;

    if (nParametri > 2) {
        printf("Errore, puoi passare al massimo due soli parametri alla funzione\n");
        exit(1);
    }

    int n;
    int F = 0;

    if (nParametri > 0) {
        if (argv[1][0] != '-') {
            printf("Errore, il primo parametro non comincia con \'-\'\n");
            exit(2);
        }

        n = atoi(&(argv[1][1]));
        if (n <= 0) {
            printf("Errore, non è stato passato un numero intero strettamente positivo\n");
            exit(3);
        }
        if (nParametri > 1) {
            close(0);
            F = open(argv[2], O_RDONLY);
            if (F != 0) {
                printf("Errore, impossibile aprire il file %s correttamente", argv[2]);
                exit(4);
            }
        }
    } else {
        n = 10;
    }

    
    int contaLinee = 0;

    while (contaLinee < n) {
        int returnRead;
        char c;

        returnRead = read(F, &c, 1);    // usare F o 0 è equivalente

        if (returnRead == 0) {
            break;
        }

        write(1, &c, 1);

        if (c == '\n') {
            contaLinee++;
        }
    }

    
    exit(0);
}