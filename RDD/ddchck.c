#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h> 

#define MAX_THREADS 10
#define MAX_MUTEX 10

int mutex_num = 0, tid_num = 0;
pthread_t tid_arr[MAX_THREADS];
pthread_mutex_t * mutex_arr[MAX_THREADS][MAX_MUTEX];
pthread_mutex_t * mutex_list[MAX_MUTEX];
int cell[MAX_MUTEX][MAX_MUTEX] = {0};

// if it was succeed return 0, otherwise return 1
int read_bytes(int fd, void * buf, size_t len) {
    char * p = buf ;
    size_t acc = 0 ;

    while (acc < len) {
        size_t _read ;
        _read = read(fd, p, len - acc) ;
        // if (_read == 0) {
        //     fprintf(stderr, "FIFO closed");
        //     return 1 ;
        // }
        p += _read ;
        acc += _read ;
    }
    return 0 ;
}

// update to element of mutex_arr, mutex_list to be null
void init_mutex_arr() {
    int i, j;
    for (i = 0; i < MAX_MUTEX; i++) {
        for (j = 0; j < MAX_MUTEX; j++) {
            mutex_arr[i][j] = NULL;
        }
    }
    for (i = 0; i < MAX_MUTEX; i++) {
        mutex_list[i] = NULL;
    }
}

// return the index of tid_arr which is same tid
int find_thread(pthread_t tid) {
    // check if tid in tid_arr
    int i; 
    for (i = 0; i < tid_num; i++) {
        if (tid == tid_arr[i]) {
            return i;
        }
    }
    // if tid doesn't exist in tid_arr, then insert;
    tid_arr[tid_num] = tid;
    tid_num++;
    return tid_num - 1;
}

// return index of mutex_list which is same with m
// also insert
int find_mutex_index(pthread_mutex_t * m) {
    int i = 0;
    while (mutex_list[i] != NULL) {
        // if m exists in mutex_list
        if (mutex_list[i] == m) {
            return i;
        }
        i++;
    }
    // if m doesnt exist, them insert
    mutex_list[i] = m;
    return i;
}

int insert_mutex(int index, pthread_mutex_t * m) {
    int i;
    // insert m to mutex_arr
    mutex_arr[index][find_mutex_index(m)] = m;
    for (i = 0; i < MAX_MUTEX; i++) {
        // make edge
        if (mutex_arr[index][i] != NULL && mutex_arr[index][i] != m) {
           cell[find_mutex_index(mutex_arr[index][i])][find_mutex_index(m)] = 1;
        }
    }
    return 0;
}

int extract_mutex(int index, pthread_mutex_t * m) {
    int i;
    // remove m from mutex_arr[index]
    mutex_arr[index][find_mutex_index(m)] = NULL;
    // remove edge
    for (i = 0; i < MAX_MUTEX; i++) {
        cell[find_mutex_index(m)][i] = 0;
        cell[i][find_mutex_index(m)] = 0;
    }
    return 0;
}

// if n in visited return 1;
int check_visited(int n, int visited[]) {
    int i = 0;
    while (visited[i] != -1) {
        if (visited[i] == n) {
            return 1;
        }
        i++;
    }
    return 0;
}

// mode 0: insert n(return 0), mode 1: extract n(return 1)
int update_visited(int n, int visited[]) {
    int i = 0;
    while (visited[i] != -1) {
        if (visited[i] == n) {
            visited[i] = -1;
            return 1;
        }
        i++;
    }
    visited[i] = n;
    return 0;
}

int dfs_check(int y, int visited[]) {
    int i;
    for (i = 0; i < MAX_MUTEX; i++) {
        if (cell[y][i] != 0) {
            // check i in visited
            if (check_visited(i, visited) == 1) {
                return 1;
            }
            // insert i to visited
            update_visited(i, visited);
            // continue finding
            if (dfs_check(i, visited) == 0) {
                // remove i from visited
                update_visited(i, visited);
                return 0;
            } else {
                return 1;
            }
        }
    }
    return 0;
}

// if cycle exist return 1, otherwise 0
int find_cycle() {
    int i, j;
    for (i = 0; i < MAX_MUTEX; i++) {
        int visited[10] = {-1};
        visited[0] = i;
        for (j = 0; j < 10; j++) {
            if (cell[i][j] != 0) {
                visited[1] = j;
                // remove j from visited
                if (dfs_check(j, visited) == 0) {
                    visited[1] = -1;
                } else {
                    return 1;
                }
            }
        }
        return 0;
    }
}

void print_cell() {
    int i, j;
    printf("------map------\n");
    for (i = 0; i < MAX_MUTEX; i++) {
        for (j = 0; j < MAX_MUTEX; j++) {
            printf("%d  ", cell[i][j]);
        }
        printf("\n");
    }
    printf("------map------\n");
}

int main (int argc, char * argv[]) {
    // remove .ddtrace fifo file
	if (unlink(".ddtrace") == 0) {
        printf(".ddtrace removed\n");
    } else {
        perror("remove .ddtrace failed\n");
    }

	 // make .ddtrace fifo
    if (mkfifo(".ddtrace", 0666)) {
		if (errno != EEXIST) {
			perror("fail to open fifo: ") ;
			exit(EXIT_FAILURE) ;
		}
	}

	FILE *fp;
    int i, index;

    init_mutex_arr();


	int fd = open(".ddtrace", O_RDONLY | O_SYNC) ;

	while (1) {
		char s[128];
		pthread_t tid;
		pthread_mutex_t * mutex;
		int len, pc_addr;
		int flag;

		// read flag
		if (read_bytes(fd, &flag, sizeof(int)) == 1) {
			// fprintf(stderr, "read flag failed\n");
			exit(EXIT_FAILURE);
		}
		fprintf(stderr, "flag: %d\n", flag);

		// read pthread
		if (read_bytes(fd, &tid, sizeof(pthread_t)) == 1) {
			fprintf(stderr, "read pthread failed\n");
			exit(EXIT_FAILURE);
		}
    	fprintf(stderr, "thread: %lu\n", tid);

		// read mutex
		if (read_bytes(fd, &mutex, sizeof(&mutex)) == 1) {
			fprintf(stderr, "read mutex addr failed\n");
			exit(EXIT_FAILURE);
		}
    	fprintf(stderr, "mutex: %p\n", (void *)mutex);
	
		// read pc addr
		if (read_bytes(fd, &pc_addr, sizeof(int)) == 1) {
			fprintf(stderr, "read pc addr failed\n");
			exit(EXIT_FAILURE);
		}
		fprintf(stderr, "+0x%x\n", pc_addr);

		// find index of tid_addr
        index = find_thread(tid);
        printf("index: %d\n", index);

		if (flag == 1) {	// locked
            // insert mutex
			insert_mutex(index, mutex);
        } else if (flag == 0) {		// unlocked
            // extract mutex
			extract_mutex(index, mutex);
        }

		print_cell();

		int result = find_cycle();
        printf("result: %d\n", result);
        printf("\n\n\n");

        if (result == 1) {
            break;
        }


	}


	close(fd) ;
	return 0 ;
}