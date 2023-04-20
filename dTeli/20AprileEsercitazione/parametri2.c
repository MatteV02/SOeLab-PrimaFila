#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc,char** argv){

	if(argc<4){
		printf("no");
		exit(1);
	}

	if(open(argv[1],O_RDONLY)<0){
		printf("hell no");
		exit(2);
	}
	int N;
	if((N=atoi(argv[2]))<=0){
		printf("nope");
		exit(3);
	}
	if(argv[3][1]!=0){
		printf("errore");
		exit(4);
	}
	char C=argv[3][0];
	printf("eseguibile: %s, numero di params %d \n",argv[0],argc-1);
	printf("file: %s",argv[1]);
	printf("numero %d",N);
	printf("carattere %c",C);
}
