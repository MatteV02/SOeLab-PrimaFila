#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <fcntl.h>
#include <sys/wait.h>
#include <time.h>

int main(int argc, char** argv){
       
    int ritorno;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
    if (wait(&ritorno)<0)
    {
        printf("Il processo figlio è stato terminato in modo anomalo\n");
        exit(1);
    }     
    printf("stampo pid %i\n",getpid());    

    return(0);
}