#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <libgen.h>
#include <errno.h>

int
main(int argc, char * argv[])
{
    get_option(argc, argv) ;
    
    /* Connect socket */

    int sock_fd = socket(AF_INET, SOCK_STREAM, 0) ;
    if (sock_fd <= 0) {
        perror("socket failed : ") ;
        exit(EXIT_FAILURE) ;
    }

    struct sockaddr_in serv_addr ;
    memset(&serv_addr, '\0', sizeof(serv_addr)) ;
    serv_addr.sin_family = AF_INET ;
    serv_addr.sin_port = htons(port_num) ;
    if (inet_pton(AF_INET, hostip, &serv_addr.sin_addr) <= 0) {
        perror("inet_pton failed : ") ;
        exit(EXIT_FAILURE) ;
    }

    if (connect(sock_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("connect failed : ") ;
        exit(EXIT_FAILURE) ;
    }

    
    /* Send the request to the server */
    
    request(sock_fd) ;
    shutdown(sock_fd, SHUT_WR) ;


    /* Receive response from the server */

    receive_response(sock_fd) ;

    // 이렇게 설정하니 receive_get_response 받기 전에 server 에서 sending 할 때 broken pipe error 뜸
    // if (ch.command == list) 
    //     receive_list_response(sock_fd) ;
    // else if (ch.command == put) 
    //     receive_get_response(sock_fd) ; 
    
    return 0 ;
}
