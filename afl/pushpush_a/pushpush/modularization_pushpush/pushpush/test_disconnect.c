#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include "include/server.h"


int main()
{
    for(int i=0; i<MAX_USER; i++)
    {
        clnt_socks[i] = open("/dev/null",O_RDONLY);
        clnt_cnt++;
    }
    char pattern[10];
    char str[2]="?";
    sprintf(pattern, "%%1[0-%d]s", MAX_USER);
    scanf(pattern, str);

    if( strcmp(str, "?") != 0)
    {
        disconnect(clnt_socks[atoi(str)]);
        printf("%d\n",atoi(str));

    }
}