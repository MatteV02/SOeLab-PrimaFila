#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
	if(argc < 2){
		printf("Errore parametri <1");
		exit(1);
	}

	for(int i=1;i<argc;i++){
		printf("parametro n.%d %s",i,argv[i]);
	}

	exit(0);
}
