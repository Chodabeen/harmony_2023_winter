// #include <stdio.h>
// #include <stdlib.h>
// #include <pthread.h>
// #include <time.h>
// #include <unistd.h>

// pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
// pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;

// void noise() {
// 	usleep(rand() % 1000) ;
// }

// void * thread1(void *arg) {
// 		pthread_mutex_lock(&mutex1);
// 		usleep(1);
// 		pthread_mutex_lock(&mutex2);
// 		pthread_mutex_unlock(&mutex1); 
// 		pthread_mutex_unlock(&mutex2); 

// 		return NULL;
// }

// void * thread2(void *arg) {
// 		pthread_mutex_lock(&mutex2);
// 		pthread_mutex_lock(&mutex1);
// 		pthread_mutex_unlock(&mutex1); 
// 		pthread_mutex_unlock(&mutex2); 

// 		return NULL;
// }

// int main(int argc, char *argv[]) {
// 	pthread_t tid1, tid2;
// 	srand(time(0x0)) ;

// 	pthread_create(&tid1, NULL, thread1, NULL);
//     pthread_create(&tid2, NULL, thread2, NULL);

// 	pthread_join(tid1, NULL);
//     pthread_join(tid2, NULL);

// 	return 0;
// }


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t mutex1, mutex2, mutex3, mutex4, mutex5;

void* threadFunction(void* arg) {
    int id = *((int*)arg);
    free(arg);

    sleep(1);

    if (id % 2 == 0) {
        pthread_mutex_lock(&mutex1);
        sleep(1);
        pthread_mutex_lock(&mutex2);
        sleep(1);
        pthread_mutex_lock(&mutex3);
        sleep(1);
        pthread_mutex_lock(&mutex4);
        sleep(1);
        pthread_mutex_lock(&mutex5);
    } else {
        pthread_mutex_lock(&mutex5);
        sleep(1);
        pthread_mutex_lock(&mutex4);
        sleep(1);
        pthread_mutex_lock(&mutex3);
        sleep(1);
        pthread_mutex_lock(&mutex2);
        sleep(1);
        pthread_mutex_lock(&mutex1);
    }

    // Do something

    pthread_mutex_unlock(&mutex1);
    pthread_mutex_unlock(&mutex2);
    pthread_mutex_unlock(&mutex3);
    pthread_mutex_unlock(&mutex4);
    pthread_mutex_unlock(&mutex5);

    return NULL;
}

int main() {
    pthread_t threads[5];

    pthread_mutex_init(&mutex1, NULL);
    pthread_mutex_init(&mutex2, NULL);
    pthread_mutex_init(&mutex3, NULL);
    pthread_mutex_init(&mutex4, NULL);
    pthread_mutex_init(&mutex5, NULL);

    for (int i = 0; i < 5; ++i) {
        int* threadId = (int*)malloc(sizeof(int));
        *threadId = i;
        pthread_create(&threads[i], NULL, threadFunction, threadId);
    }

    sleep(10);

    for (int i = 0; i < 5; ++i) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex1);
    pthread_mutex_destroy(&mutex2);
    pthread_mutex_destroy(&mutex3);
    pthread_mutex_destroy(&mutex4);
    pthread_mutex_destroy(&mutex5);

    return 0;
}
