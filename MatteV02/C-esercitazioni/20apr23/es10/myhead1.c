#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    if (argc != 2) {
        printf("Errore, devi passare un solo parametro alla funzione\n");
        exit(1);
    }

    if (argv[1][0] != '-') {
        printf("Errore, il secondo parametro non comincia con \'-\'\n");
        exit(2);
    }

    int n = atoi(&(argv[1][1]));
    if (n <= 0) {
        printf("Errore, non è stato passato un numero intero strettamente positivo\n");
        exit(3);
    }

    int contaLinee = 0;

    while (contaLinee < n) {
        int returnRead;
        char c;

        returnRead = read(0, &c, 1);

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