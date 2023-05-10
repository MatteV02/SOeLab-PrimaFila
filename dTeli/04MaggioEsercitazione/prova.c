#include <stdio.h>	// Includo la libreria per la funzione printf
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close e fork
#include <fcntl.h>	// Includo la libreria per la funzione open e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait


int main(int argc, char** argv) {
    int c=0;
    printf("richiamare la ricorsiva se no inserire 0");
    scanf("%d",&c);
    if(c!=0){
        execlp("ls","ls",(char *) 0);
    }
    exit(0);
}