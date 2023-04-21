#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char **argv)
{       int nread;                      /* valore ritorno read */
	char buffer[BUFSIZ];            /* usato per i caratteri */
	int fd = 0;			/* usato per la open; N.B. se non viene passato alcun parametro allora rimarra' uguale a 0! */
					/* quindi identifichera' lo STANDARD INPUT */

    int nfile = 1;

	if (argc > 1)
	/* abbiamo un parametro che deve essere considerato il nome di un file */
    {
        nfile = argc - 1;
    }
    

    for (int i = 0; i < nfile; i++) {
        if (argc != 1) {
            fd = open(argv[i + 1], O_RDONLY);
        }

        /* se non abbiamo un parametro, allora fd rimane uguale a 0 */
        /* lettura dal file o dallo standard input fino a che ci sono caratteri */
        while ((nread = read(fd, buffer, BUFSIZ)) > 0 )
            /* scrittura sullo standard output dei caratteri letti */
                    write(1, buffer, nread);
    }

	exit(0);
}