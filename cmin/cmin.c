// #include <stdio.h>
// #include <string.h>
// #include <stdlib.h>
// #include <signal.h>
// #include <unistd.h>
// #include <fcntl.h>
// #include <sys/wait.h>
// #include <sys/time.h>
// #include <getopt.h>

// int input_pipes[2], output_pipes[2];
// char str_err[128], output_file[128], target_pg[128], options[64];
// pid_t child_pid ;
// struct itimerval t ;

// // if it was succeed return 0, otherwise return 1
// int read_bytes(int fd, void * buf, size_t len) {
//     char * p = buf ;
//     size_t acc = 0 ;

//     while (acc < len) {
//         size_t _read ;
//         _read = read(fd, p, len - acc) ;
//         if (_read == 0) {
//             return 1 ;
//         }
//         p += _read ;
//         acc += _read ;
//     }
//     return 0 ;
// }

// // if it was succeed return 0, otherwise return 1
// int write_bytes(int fd, void * buf, size_t len) {
//     char * p = buf ;
//     size_t acc = 0 ;

//     while (acc < len) {
//         size_t written ;
//         written = write(fd, p, len - acc) ;
//         if (written == 0) {
//             return 1 ;
//         }
//         p += written ;
//         acc += written ;
//     }
//     return 0 ;
// }

// void handler(int sig) {
//     fprintf(stderr, "i will kill your child\nTime out\n");
// 	kill(child_pid, SIGTERM); 
//     exit(EXIT_FAILURE);
// }

// void run_target() {
//     close(input_pipes[1]) ;
//     close(output_pipes[0]) ;
//     dup2(input_pipes[0], 0) ;
//     dup2(output_pipes[1], 2) ;
//     close (STDOUT_FILENO);

//     execlp(target_pg, target_pg, options, (char *)NULL);
    
// 	exit(0) ;
// }

// // if crash exists return 1, otherwise 0
// int check_crashing() {
//     char buf[4097];
//     read_bytes(output_pipes[0], buf, 4097);
    
//     if(strstr(buf,str_err) != NULL){
//         return 1;
//     }
//     else{
//         return 0;
//     }
// }

// void _strncpy(char * s1, char * s2, size_t cp_n) {
//     // fprintf(stderr, "s1: %s\n s2: %s\n cp_n: %ld\n", s1, s2, cp_n);
//     if (cp_n == 0) {
//         return;
//     }
//     int i;
//     for (i = 0; i < cp_n; i++) {
//         s1[i] = s2[i];
//         // fprintf(stderr, "s1...: %s\n", s1);
//     }
//     // fprintf(stderr, "s1: %s\n", s1);
// }

// void _strcat(char * dest, char * src, size_t dest_n, size_t src_n) {
//     if (src_n == 0) {
//         return;
//     }
//     int i; 
//     for (i = 0; i < src_n; i++) {
//         dest[dest_n + i] = src[i];
//     }
//     // dest[i] = '\0';
// }

// char * reduce(int len, char * t) {
//     int exit_code ;
//     int i;
    
//     int s = len - 1;
//     // fprintf(stderr, "%d %d\n", len, s);
//     while (s > 0) {
//         fprintf(stderr, "s: %d\n", s);

//         char * head = (char*) malloc(sizeof(char) * len);
//         char * tail = (char*) malloc(sizeof(char) * len);

//         // if (s == 853) {
//         //     fprintf(stderr, "853: %s\n", t);
//         // }

//         for (i = 0; i <= len - s; i++) {
//             // make pipes
//             if (pipe(input_pipes) != 0) {
//                 perror("Error") ;
//                 exit(EXIT_FAILURE) ;
//             }
//             if (pipe(output_pipes) != 0) {
//                 perror("Error") ;
//                 exit(EXIT_FAILURE) ;
//             }

//             // strncpy(head, t, i - 1 + 1);
//             _strncpy(head, t, i - 1 + 1);
//             // fprintf(stderr, "head: %s\n", head);
//             // strncpy(tail, t + i + s, len - 1 - i - s + 1);
//             _strncpy(tail, t + i + s, len - 1 - i - s + 1);
//             // fprintf(stderr, "tail: %s\n", tail);
//             head[i - 1 + 1] = '\0';
//             tail[len - 1 - i - s + 1] = '\0';
//             // strcat(head, tail);
//             // if (s == 853) {
//             //     fprintf(stderr, "head: %ld\n", strlen(head));
//             //     fprintf(stderr, "tail: %ld\n", strlen(tail));
//             // }
//             _strcat(head, tail, i - 1 + 1, len - 1 - i - s + 1);

//             // fprintf(stderr, "head + tail: %s\n", head);

//             // run target using head+tail
//             if ((child_pid = fork()) == 0) {
//             	run_target();
//             } else {    
//                 // parent
//                 close (input_pipes[0]);
//                 close (output_pipes[1]);
//                 // start timer
//                 alarm(3);
//                 // write head+tail to input_pipes[1]
//                 write_bytes(input_pipes[1], head, len - s);
//                 close (input_pipes[1]);
//                 waitpid(child_pid, &exit_code, 0) ;
//                 // quit alarm
//                 alarm(0);
//                 if (WIFSIGNALED(exit_code) && WTERMSIG(exit_code) == SIGKILL) {
//                     printf("Parent: Child process was killed by SIGKILL.\n");
//                     exit(EXIT_FAILURE);
//                 }
//             }
//             // check head+tail satisfies crash
//             if (check_crashing() == 1) {
//                 fprintf(stderr, "REDUCE---HEAD+TAIL\n");
//                 close (output_pipes[0]);
//                 free (t);
//                 free (tail);
//                 return reduce(len - s, head);    // current head has head+tail
//             }
//             close (output_pipes[0]);
//         }

//         free(head);
//         free(tail);
//         char * mid = (char*) malloc(sizeof(char) * 4097);

//         for (i = 0; i <= len - s; i++) {
//             // make pipes
//             if (pipe(input_pipes) != 0) {
//                 perror("Error") ;
//                 exit(EXIT_FAILURE) ;
//             }
//             if (pipe(output_pipes) != 0) {
//                 perror("Error") ;
//                 exit(EXIT_FAILURE) ;
//             }

//             // strncpy(mid, t + i, s);
//             _strncpy(mid, t + i, s);
//             mid[s] = '\0';

//             // fprintf(stderr, "mid: %s\n", mid);

//             // run target using mid
//             if ((child_pid = fork()) == 0) {
//             	run_target();
//             } else {    
//                 // parent
//                 close (input_pipes[0]);
//                 close (output_pipes[1]);
//                 // start timer
//                 alarm(3);
//                 // write mid to input_pipes[1]
//                 write_bytes(input_pipes[1], &mid, s);
//                 // exit(EXIT_FAILURE);
//                 close (input_pipes[1]);
//                 waitpid(child_pid, &exit_code, 0) ;
//                 // quit alarm
//                 alarm(0);
//                 if (WIFSIGNALED(exit_code) && WTERMSIG(exit_code) == SIGKILL) {
//                     printf("Parent: Child process was killed by SIGKILL.\n");
//                     exit(EXIT_FAILURE);
//                 }
//             }
//             // check mid satisfies crash
//             if (check_crashing() == 1) {
//                 fprintf(stderr, "REDUCE---MID\n");
//                 close (output_pipes[0]);
//                 free (t);
//                 return reduce(s, mid);    // current head has head+tail
//             }
//             close (output_pipes[0]);
//         }
//         free(mid);
//         s -= 1;
//     }
//     fprintf(stderr, "end\n");
//     return t;
// }

// char * minimize(int file_size, char * t) {
//     return reduce(file_size, t);
// }

// int main(int argc, char * argv[]) {

//     int opt;
//     int i_flag = 0, m_flag = 0, o_flag = 0;
//     // input_file: crash, str_err: SEGV, output_file: reduced
//     char input_file[128];

//     while ((opt = getopt(argc, argv, "i:m:o:")) != -1) {
//         switch (opt) {
//             case 'i':
//                 strcpy(input_file, optarg);
//                 i_flag = 1;
//                 printf("i: %s\n", input_file);
//                 break;
//             case 'm':
//                 strcpy(str_err, optarg);
//                 m_flag = 1;
//                 printf("m: %s\n", str_err);
//                 break;
//             case 'o':
//                 strcpy(output_file, optarg);
//                 o_flag = 1;
//                 printf("o: %s\n", output_file);
//                 break;
//         }
//     }

//     strcpy(target_pg, argv[7]);
//     if (argc == 9) {
//         strcpy(options, argv[8]);
//         fprintf(stderr, "opt: %s\n", options);
//     }

//     // check option
//     if (!i_flag || !m_flag || !o_flag) {
//         fprintf(stderr, "Error: Missing required options\n");
//         fprintf(stderr, "Usage: %s -i input_file -m str_err -o output_file\n", argv[0]);
//         exit(EXIT_FAILURE);
//     }

//     signal(SIGALRM, handler) ;

//     // read input_file and save them to buf
//     char  * buf = (char*) malloc(sizeof(char) * 4097);
//     int input_fd = open(input_file, O_RDONLY);
//     int file_size = read(input_fd, buf, 4096);

//     close (input_fd);

//     // open output_file and write reduced sentence
//     int output_fd = open(output_file, O_CREAT | O_WRONLY | O_TRUNC, 0644);
//     fprintf(stderr, "reduced: %s\n", minimize(file_size, buf));
//     // write_bytes(output_fd, minimize(file_size, buf), strlen(minimize(buf)));
//     close(output_fd);
// }


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#define BUF_SIZE 4096
#define READ 0
#define WRITE 1
#ifdef DEBUG
    #define debug(fn) fn
#else
    #define debug(fn)
#endif

unsigned char *input = NULL;
unsigned char *error_keyword = NULL;
unsigned char *output = NULL;
unsigned char *target = NULL;
int args_size= 0;
char *args[16];
pid_t pid = -1;

int origin_size = 0;
int tm_size = 0;
unsigned char* tm = NULL;

int LETMESEE = 0;



void print(unsigned char* string, int len){
        for(int i = 0; i < len; i++) printf("%c", string[i]);
}

void save_result_as_file(){
        FILE* fp = fopen(output, "w");
        for(int i = 0; i < tm_size; i++) fprintf(fp, "%c", tm[i]);
        fclose(fp);
}

//make_result: print final size and save result as file
void make_result(){
        save_result_as_file();
        debug(
        );
        printf("\n===CIMIN=========================================\n");
        print(tm, tm_size);
        printf("\n=================================================\n\n");
        printf("(%d) -> (%d)\n", origin_size, tm_size);
        printf("reduced crash input saved in file \"%s\"\n", output);

}


//quit: signal handler for SIGINT
void quit(int sig){
        fprintf(stderr, "program terminate...\n");
        make_result();
        exit(EXIT_SUCCESS);
}

//timeout: signal handler for SIGALARM
void timeout(int sig){
        fprintf(stderr, "TIMEOUT OCURRED!\n");
    kill(pid, SIGTERM);
        quit(0);
}

//printUsage: print usage
void printUsage(unsigned char* binary){
        fprintf(stderr, "Usage: %s -i [crashed_input_filepath] -m [error_msg_keyword] \n        -o [output_filepath] [target_binary_name] (target_arg) ...\n", binary);
}

//parseArgs: parse args and save it to corresponding variables
void parseArgs(int argc, char *argv[]){

        int opt;
    while ((opt = getopt(argc, argv, "i:m:o:")) != -1) {
        switch (opt) {
            case 'i':
                input = optarg;
                break;
            case 'm':
                error_keyword = optarg;
                break;
            case 'o':
                output = optarg;
                break;
                        case '?':
                        default:
                                printUsage(argv[0]);
                exit(EXIT_FAILURE);
        }
    }

        if (input == NULL || error_keyword == NULL || output == NULL) {
                printUsage(argv[0]);
            exit(EXIT_FAILURE);
        }
    for (int i = optind; i < argc; i++) {
                if(i == optind) target = argv[i];
                args[args_size++] = argv[i]; 
    }
        args[args_size] = NULL;

        if(strstr(args[0], " ") != NULL){
                unsigned char* tok = (args[args_size++] = strtok(args[0], " "));
                while(tok != NULL) tok = (args[args_size++] = strtok(NULL, " "));
        }

        debug(
                printf("Input: %s\n", input);
        printf("Error Keyword: %s\n", error_keyword);
        printf("Output: %s\n", output);
        printf("Target: %s\n", target);
                for(int i = 0; i < args_size; i++) printf("args %d: %s\n", i+1, args[i]);
                printf("\n");
        );
        return;

}


//is_crashed: exec target program with input crash as stdin.
//return 0 if theres no crash, return 1 if there was crash
int is_crashed(unsigned char* crash, int size){

        int p2c[2], c2p[2];
        if((pipe(p2c) < 0) || (pipe(c2p) < 0)){
                perror("pipe()");
                exit(EXIT_FAILURE);
        }

        pid = fork();

        if(LETMESEE) printf("[is_crashed: %d]   make pipe...\n", LETMESEE);

        if(pid < 0){                            //error
                perror("fork");
                exit(EXIT_FAILURE);
        }else if(pid > 0){                      //parents
                debug(fprintf(stderr, "parent start\n"););
                close(p2c[READ]);
                close(c2p[WRITE]);
                alarm(3);

                //send crash input as stdin of child
                write(p2c[WRITE], crash, size);
                close(p2c[WRITE]);
        if(LETMESEE) printf("   [is_crashed: %d] parents write...\n", LETMESEE);
                waitpid(pid, NULL, 0);
                alarm(0);

                //read output from child stderr
                unsigned char buf[BUF_SIZE];
                int len, error;
                while((len = read(c2p[READ], buf, BUF_SIZE-1)) > 0){
                        buf[len] = 0;
                        debug(printf("\nparents recv: %s\n", buf););
                        //determine if error keyword we find is exist
                        if(strstr(buf, error_keyword) != NULL) error = 1;
                        else error = 0;
                }
        if(LETMESEE) printf("   [is_crashed: %d] parents read...\n", LETMESEE);
                close(c2p[READ]);

                debug(fprintf(stderr, "parent end\n"););
                return error;

        }else{                                          //child
                debug(fprintf(stderr, "child start\n"););
                close(p2c[WRITE]);
                close(c2p[READ]);

                //redirect some std pipes to our unnamed pipes
                dup2(p2c[READ], STDIN_FILENO);
                dup2(c2p[WRITE], STDERR_FILENO);
                close(STDOUT_FILENO); 
        if(LETMESEE) printf("   [is_crashed] child dup...\n");

                unsigned char buf[BUF_SIZE];
                execv(target, args);

                close(p2c[READ]);
                close(c2p[WRITE]);
                debug(fprintf(stderr, "child end\n"););
                exit(EXIT_SUCCESS);

        }
        return 0;

}


//make_substr: make substring of src to dest, range of start-end index, and return size of substr
int make_substr(unsigned char* src, unsigned char* dest, int start, int end){
        int index = 0;
        for(int i = start; i < end; i++) dest[index++] = src[i];
        return index;

}

//cat_strings: concatnate second string to first string, and return first string
unsigned char* cat_strings(unsigned char* first, unsigned char* second, int firstsize, int secondsize){
        for(int i = 0; i < secondsize; i++) first[firstsize + i] = second[i];
        return first;
}



//reduce: reduce crash input and return it
unsigned char* reduce(unsigned char* origin, int len){

        //TODO idk if its right...
        /*
        unsigned char head[BUF_SIZE], tail[BUF_SIZE], mid[BUF_SIZE];
        */
        unsigned char *head = (unsigned char*)malloc(sizeof(unsigned char)*len + 1);
        unsigned char *tail = (unsigned char*)malloc(sizeof(unsigned char)*len + 1);
        unsigned char *mid = (unsigned char*)malloc(sizeof(unsigned char)*len + 1);

        debug(printf("start to reduce size %d...\n", len););
        printf("start to reduce size %d...\n", len);
        save_result_as_file();
        //const int POINT = 1729;

        int s = len - 1;
        //if(s < POINT) LETMESEE = s;

        while(s > 0){
                debug(printf("\n## src: %s (%d)\n", origin, s););
                if(s%10 == 0) printf("  while with %d\n", s);
        if(LETMESEE) printf("   before check head+tail...\n");
                for(int i = 0; i <= (len - s); i++){
                        int headlen = make_substr(origin, head, 0, i);
        if(LETMESEE) printf("   make head...\n");
                        int taillen = make_substr(origin, tail, (s+i), len);
        if(LETMESEE) printf("   make tail...\n");
                        debug(printf("%d)\n     head : %s \n    tail: %s\n", i, head, tail););
                        cat_strings(head, tail, headlen, taillen);
        if(LETMESEE) printf("   meow meow...\n");
                        if(headlen+taillen == 0) continue;
                        debug(printf("  meow: %s\n", head););
                        if(is_crashed(head, headlen+taillen)){
                                tm = head;
                                tm_size = headlen+taillen;
                                /*
                                */
                                free(tail);
                                free(mid);
                                free(origin);
                                return reduce(head, headlen+taillen);
                        }
        if(LETMESEE) printf("   check if crashed...\n");
                }
        if(LETMESEE) printf("   before check min...\n");
                for(int i = 0; i <= (len - s); i++){
                        int midlen = make_substr(origin, mid, i, (i+s));
                        if(midlen == 0) continue;
                        debug(printf("  mid: %s\n", mid););
                        if(is_crashed(mid, midlen)){
                                tm = mid;
                                tm_size = midlen;
                                /*
                                */
                                free(head);
                                free(tail);
                                free(origin);
                                return reduce(mid, midlen);
                        }
                }
                s--;
        }
        return origin;

}

//this is MAIN
int main(int argc, char *argv[]) {
//      unsigned char buf[BUF_SIZE];
        unsigned char* buf = (unsigned char*) malloc(sizeof(unsigned char) * BUF_SIZE + 1);

        parseArgs(argc, argv);
        signal(SIGALRM, timeout);
        signal(SIGINT, quit);

        //get initial crash input
        int fd = open(input, O_RDONLY);
        origin_size = read(fd, buf, BUF_SIZE);
        if(buf[origin_size-2] == '\n') buf[(origin_size--)-2] = 0;

        tm = buf;
        tm_size = origin_size;
        reduce(buf, origin_size);

        make_result();
    return 0;
}