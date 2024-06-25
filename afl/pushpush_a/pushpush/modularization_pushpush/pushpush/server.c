#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <pthread.h>
#include "../cJSON.h"

#define MAX_USER 4
#define NAME_SIZE 16
#define PATH_LENGTH 256

int usr_cnt = 0; //num of connected user
int game_start = 0;
int max_user;
char ** user_name;
int json_size;
char * json_serialize;

int clnt_cnt = 0;
int clnt_socks[MAX_USER];
pthread_mutex_t mutx;
//error_handling: print error message
void error_handling(char * msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}

//this is MAIN function
int main(int argc, char *argv[])
{
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_adr, clnt_adr;
	unsigned int clnt_adr_sz;
	pthread_t t_id;
	if (argc != 2) {
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
	serv_sock = socket(PF_INET, SOCK_STREAM, 0);

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET; 
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(atoi(argv[1]));
	
	if (bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr)) == -1)
		error_handling("bind() error");
	
	if (listen(serv_sock, 5) == -1)
		error_handling("listen() error");
		
    clnt_adr_sz = sizeof(clnt_adr);
	clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
	
		int a;
		int size = read(clnt_sock,(void*)&a,sizeof(int));
        printf("read size %d, data %d\n",size,a);

        // sleep(3);
        // close(clnt_sock);
        size = read(clnt_sock,(void*)&a,sizeof(int));
        printf("read size %d, data %d\n",size,a);
		size = read(clnt_sock,(void*)&a,sizeof(int));
        printf("read size %d, data %d\n",size,a);

		// size = write(clnt_sock,(void*)&a,sizeof(int));
    	// printf("write size %d\n",size);
		

        
		
	
	// free(json_serialize);
	close(serv_sock);
	return 0;
}
	
