#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> 
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>


int main(int argc, char *argv[]) {


	struct sockaddr_in serv_addr;

	int sock = socket(PF_INET, SOCK_STREAM, 0);
	
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));
	  
	if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
	{
		fprintf(stderr, "ERROR: connect() error\n");
		exit(1);
	}

    char buf[10];
	write(sock, buf , 0);
	write(sock, buf , 0);

	write(sock, buf , 0);
	write(sock, buf , 0);

	write(sock, buf , 3);
	write(sock, buf , 0);




 	return 0;
}
