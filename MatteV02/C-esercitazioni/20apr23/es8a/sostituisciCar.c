#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char** argv) {
    int nParametri = argc - 1;

    if (nParametri != 2) {
        printf("Errore, il programma si aspetta 2 parametri passati\n");
        exit(1);
    }

    int F = open(argv[1], O_RDWR);
    if (F < 0) {
        printf("Errore in apertura del file %s\n", argv[1]);
        exit(2);
    }

    if (strlen(argv[2]) != 1) {
        printf("Errore, il secondo parametro dovrebbe essere un carattere singolo\n");
        exit(3);
    }
    char Cx = argv[2][0];

    while(1) {
        char c;
        
        if (read(F, &c, 1) == 0) {
            break;
        }

        if (c == Cx) {
            lseek(F, -1, SEEK_CUR);
            write(F, " ", 1);
        }
    }

    close(F);
    exit(0);
}