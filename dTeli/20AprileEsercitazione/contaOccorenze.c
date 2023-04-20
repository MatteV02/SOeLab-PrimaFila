#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <fcntl.h>

int main(int argc, char** argv){
	if (argc != 3){
		printf("errore nei tre parametri");
		exit(1);
	}
	int fd;
	if((fd=open(argv[1],O_RDONLY))<0){
		printf("non un file");
		exit(2);
	}

	if(argv[2][1]!=0){
		printf("deve essere un carattere");
		exit(3);
	}
	char Cx=argv[2][0];
	char dato;
	int conta=0;
	int nread;
	while((nread=read(fd,&dato,1))<0){
		if(dato==Cx){
			conta++;
		}
	}
	printf("il contro e $d",conta);
	exit(0);
}
