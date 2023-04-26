#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
    int nParams = argc - 1;
    
    if (nParams != 2)
    {
        printf("Errore, devi passare 2 soli parametri\n");
        exit(1);
    }

    int F = open(argv[1], O_RDONLY);
    if (F < 0)
    {
        printf("Errore, il primo parametro non corrisponde ad un file leggibile\n");
        exit(2);
    }

    int n = atoi(argv[2]);
    if (n <= 0)
    {
        printf("Errore, il secondo parametro non corrisponde ad un numero strettamente positivo");
        exit(3);
    }

    char *buffer = malloc((n + 1) * sizeof(*buffer));
    int j = 0;

    while (read(F, &(buffer[j]), 1 * sizeof(*buffer)) != 0)
    {
        if (buffer[j] == '\n')
        {
            if (n == j + 1)
            {
                buffer[j + 1] = 0;
                printf("Travata una riga di lunghezza pari a %d: %s", n, buffer);
            }
            j = 0;
        }
        else
        {
            j++;
        }
    }

    free(buffer);

    exit(0);
}