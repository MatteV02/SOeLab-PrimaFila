/* FILE: provaPipe.c */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main (int argc, char* argv[])
{  	
    if (argc != 3) /* controllo sul numero di parametri: devono essere in numero uguale a 2*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }
     int fd1, fd2;
    if ((fd1 = open(argv[1], O_RDONLY)) < 0)  	/* dobbiamo sempre controllare il valore di ritorno delle open: ATTENZIONE ALLE PARENTESI! */
	{ 	printf("Errore in apertura file per %s dato che fd1 = %d\n", argv[1], fd1);
        	exit(2); 
	}	
	/* NOTA BENE: non serve l'else perche' in caso di errore si fa la exit! */
	printf("Valore di fd1 = %d per %s\n", fd1, argv[1]);

	if ((fd2 = open(argv[2], O_RDONLY)) < 0)	/*  apriamo un altro file per verificare che verra' usato un altro elemento della tabella dei file aperti del singolo processo! */
	{ 	printf("Errore in apertura file per %s dato che fd2 = %d\n", argv[2], fd2);
        	exit(3); 
	}
	printf("Valore di fd2 = %d per %s\n", fd2, argv[2]);

	close(fd1);	/* chiudiamo e quindi eliminiamo le informazioni relative all'elemento della tabella dei file aperti identificato da fd1 */

    int piped[2]; 	/* array di due interi per la pipe */

	/* si crea una pipe */
	if (pipe(piped) < 0 ) 
	{ 
               	printf("Errore nella creazione pipe\n");
		exit(1); 
	}
    
	printf("creato pipe con piped[0]= %d \n", piped[0]);
	printf("creato pipe con piped[1]= %d \n", piped[1]);

	exit(0);
}