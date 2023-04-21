#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    int N = argc - 1;
    
    if (argc < 2) {
        printf("Errore, devi passare almeno 2 parametri\n");
        exit(1);
    }

    printf("Il nome dell'eseguibile e': %s\n", argv[0]);
    printf("Il numero di parametri passati e': %d\n", N);

    for(int i = 0; i < N; i++) {
        printf("Il parametro %d e': %s\n", i, argv[i + 1]);
    }
    exit(0);
}