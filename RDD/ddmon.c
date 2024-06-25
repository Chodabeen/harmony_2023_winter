#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <execinfo.h>
#include <pthread.h> 
#include <sys/types.h>
#include <sys/stat.h>

pthread_mutex_t d_mutex = PTHREAD_MUTEX_INITIALIZER; 

// if it was succeed return 0, otherwise return 1
int write_bytes(int fd, void * buf, size_t len) {
    char * p = buf ;
    size_t acc = 0 ;

    while (acc < len) {
        size_t written ;
        written = write(fd, p, len - acc) ;
        if (written == 0) {
            return 1 ;
        }
        p += written ;
        acc += written ;
    }
    return 0 ;
}


int pthread_mutex_lock (pthread_mutex_t * mutex) {
	int (*lockcp)(pthread_mutex_t *mutex) ; 
    int (*unlockcp)(pthread_mutex_t *mutex) ; 
	char * error ;
    char * addr ,* ptr;
    char cmd[128];
    int flag = 1;    // 1 means locked

    int i, result;
    void * arr[10] ;
    char ** stack ;
	
	lockcp = dlsym(RTLD_NEXT, "pthread_mutex_lock") ;
    unlockcp = dlsym(RTLD_NEXT, "pthread_mutex_unlock") ;

	if ((error = dlerror()) != 0x0) 
		exit(EXIT_FAILURE) ;

     // open fifo
    if (mkfifo(".ddtrace", 0666)) {
		if (errno != EEXIST) {
			perror("fail to open fifo: ") ;
			exit(EXIT_FAILURE) ;
		}
	}

    int fd = open(".ddtrace", O_WRONLY | O_SYNC) ;

    // ---------------lock-----------------
    lockcp(&d_mutex);

    fprintf(stderr, "Lock\n");

    // send 1 byte. flag for if locked or unlocked
    if (write_bytes(fd, &flag, sizeof(int)) == 1) {
        fprintf(stderr, "write flag failed\n");
        exit(EXIT_FAILURE);
    }

    pthread_t p = pthread_self();

    // send pthread
    if (write_bytes(fd, &p, sizeof(pthread_t)) == 1) {
        perror("write pthread failed");
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "thread: %lu\n", pthread_self());

    // send mutex
    if (write_bytes(fd, &mutex, sizeof(&mutex)) == 1) {
        perror("write mutex failed");
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "mutex: %p\n", (void *)mutex);

    size_t sz = backtrace(arr, 10) ;
    stack = backtrace_symbols(arr, sz) ;

    int decimal_addr;

    fprintf(stderr, "Stack trace\n") ;
    fprintf(stderr, "============\n") ;
    for (i = 0 ; i < sz ; i++) {
        fprintf(stderr, "[%d] %s\n", i, stack[i]) ;
        if (strncmp("./target", stack[i], 8) == 0) {
            addr = strchr(stack[i], '(');
            ptr = strtok(addr + 1, ")");
            // -4 byte from ptr
            sscanf(ptr + 1, "%x", &decimal_addr);
            decimal_addr -= 4;
            sprintf(cmd, "locked: +0x%x\n", decimal_addr);
        }
    }
    fprintf(stderr, "============\n\n") ;

    // send pc addr
    if (write_bytes(fd, &decimal_addr, sizeof(int)) == 1) {
        perror("write pc addr failed");
        exit(EXIT_FAILURE);
    }

    unlockcp(&d_mutex);
    // ---------------unlock-----------------

    close(fd);
    
	return lockcp(mutex) ; 	
}

int pthread_mutex_unlock (pthread_mutex_t *mutex)
{
	int (*lockcp)(pthread_mutex_t *mutex) ; 
    int (*unlockcp)(pthread_mutex_t *mutex) ; 
	char * error ;
    char * addr , * ptr;
    int flag = 0;    // 0 means unlocked
    char cmd[128];

    int i, result;
    void * arr[10] ;
    char ** stack ;
	
	lockcp = dlsym(RTLD_NEXT, "pthread_mutex_lock") ;
    unlockcp = dlsym(RTLD_NEXT, "pthread_mutex_unlock") ;

	if ((error = dlerror()) != 0x0) 
		exit(EXIT_FAILURE) ;

    int fd = open(".ddtrace", O_WRONLY | O_SYNC) ;

    // ---------------lock-----------------
    lockcp(&d_mutex);

    fprintf(stderr, "Unlock\n");

    // send 4 byte. flag for if locked or unlocked
    if (write_bytes(fd, &flag, sizeof(int)) == 1) {
        fprintf(stderr, "write flag failed\n");
        exit(EXIT_FAILURE);
    }

    pthread_t p = pthread_self();

    // send pthread
    if (write_bytes(fd, &p, sizeof(pthread_t)) == 1) {
        perror("write pthread failed");
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "thread: %lu\n", pthread_self());

    // send mutex
    if (write_bytes(fd, &mutex, sizeof(&mutex)) == 1) {
        perror("write mutex failed");
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "mutex: %p\n", (void *)mutex);
    

    size_t sz = backtrace(arr, 10) ;
    stack = backtrace_symbols(arr, sz) ;

    int decimal_addr;

    fprintf(stderr, "Stack trace\n") ;
    fprintf(stderr, "============\n") ;
    for (i = 0 ; i < sz ; i++) {
        fprintf(stderr, "[%d] %s\n", i, stack[i]) ;
        if (strncmp("./target", stack[i], 8) == 0) {
            addr = strchr(stack[i], '(');
            ptr = strtok(addr + 1, ")");
            // -4 byte from ptr
            sscanf(ptr + 1, "%x", &decimal_addr);
            decimal_addr -= 4;
            sprintf(cmd, "unlocked: +0x%x\n", decimal_addr);
        }
    }
    fprintf(stderr, "============\n\n") ;

    // send pc addr
    if (write_bytes(fd, &decimal_addr, sizeof(int)) == 1) {
        perror("write pc addr failed");
        exit(EXIT_FAILURE);
    }

    unlockcp(&d_mutex);
    // ---------------unlock-----------------

    close (fd);
	
	return unlockcp(mutex) ; 
}