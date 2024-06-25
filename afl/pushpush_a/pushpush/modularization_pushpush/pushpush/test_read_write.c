#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "include/server.h"

int main()
{
    char buf[4000];
    int a=49; //ascii
    // // int read_size = read_byte(0,(void*)&a,sizeof(int));
    // int write_size = write_byte(STDOUT_FILENO,(void*)&a,sizeof(int));

    int read_size = read_byte(0,buf,4);
    int write_size = write_byte(1,buf,4);
    

    printf("%d\n",write_size);
}