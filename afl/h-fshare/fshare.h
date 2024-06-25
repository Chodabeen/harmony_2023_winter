#ifndef GOOGLE_TEST_SUM_H
#define GOOGLE_TEST_SUM_H

// C++인 gtest에서 참조/빌드 되기 위해 추가되는 구문
#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    list,
    get,
    put,
    N_cmd
} cmd ;

extern char * cmd_str[N_cmd];

typedef struct {
    cmd command ;
    int src_path_len ;
    int des_path_len ;
    int payload_size ;
} client_header ;

typedef struct {
    int is_error ; // on success 0, on error 1
    int payload_size ;
} server_header ;

extern char *recv_payload;
extern char *send_payload;

extern client_header ch;
extern server_header sh;

extern char *hostip;
extern int port_num;
extern char *file_path;
extern char *dest_dir;

extern const int buf_size;

int send_bytes(int fd, char * buf, size_t len);
cmd get_cmd_code (char * s);
void print_usage() ;
void get_option(int argc, char * argv[]);
char * parse_directory(char * toparse) ;
void request(const int sock_fd);
void receive_list_response(int sock_fd);
void make_directory(char * towrite);
void receive_get_response(int sock_fd);
void receive_put_response(int sock_fd);
void receive_response(int sock_fd);

// C++인 gtest에서 참조/빌드 되기 위해 추가되는 구문
#ifdef __cplusplus
}
#endif

#endif //GOOGLE_TEST_SUM_H