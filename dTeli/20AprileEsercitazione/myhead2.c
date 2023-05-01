#include <stdio.h>	// Includo la libreria per la funzione printf
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close e fork
#include <fcntl.h>	// Includo la libreria per la funzione open e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait


int main(int argc, char** argv) {
    if (argc >= 2) /* controllo sul numero di parametri: devono essere in numero minore o uguale a 1*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }

    int n=0;		/*Inizializzo varibile che conterra' argv[1][1]*/
    if(argc==2){
        if(argv[1][0]!='-'){
            printf("Errore, parametro errato %i\n",argv[1][0]);
            exit(2);
        }
        //controllo utlizzando la funzione atoi se il argv[1][1] e un numero strettamente positivo
        if((n=argv[1][1]-48) <= 0){
            printf("il parametro %d non un numero positivo\n",argv[1][1]);
            exit(5);
        }
    }
    else{
        n=10;
    }

    char c;
    int contaCR=0;
    while (read(0, &c, 1) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
    {
        if(contaCR==n){
            break;
        }
        printf("%c",c);
        if(c=='\n'){
            contaCR++;
        }
        
    }
    exit(0);
}