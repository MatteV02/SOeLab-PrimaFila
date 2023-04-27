#include <stdio.h>	// Includo la libreria per la funzione printf
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close e fork
#include <fcntl.h>	// Includo la libreria per la funzione open e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait


int main(int argc, char** argv) {
    if (wait((int*) 0) < 0) {
        printf("Non e' stato creato nessun processo figlio\n");
        exit(1);
    }

    if (wait((int*) 0) < 0) {
        printf("Non e' stato creato nessun processo figlio\n");
        exit(2);
    }

    exit(0);
}