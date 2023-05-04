#include <stdio.h>	// Includo la libreria per la funzione printf
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close e fork
#include <fcntl.h>	// Includo la libreria per la funzione open e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <errno.h>


int main(int argc, char** argv) {
    int input;

    printf("vuoi eseguire ricorsivamente questa applicazione? (0: no; 1: si)> ");
    scanf("%d", &input);

    if (input != 0) {
        execl("prova", "prova", (char*) 0);
        /* si esegue l'istruzione seguente SOLO in caso di fallimento */
        
        perror("errore esecuzione comando"); // #include <errno.h>
        exit(errno);
    }

    exit(0);
}