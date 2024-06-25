#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "include/server.h"

int main()
{
	pthread_mutex_init(&mutx, NULL);
    
    for(int i=0; i<MAX_USER; i++)
    {
        clnt_socks[i] = 1;
        clnt_cnt++;
    }

    char str[2]="?";
    scanf("%1[0-9]s", str);


    if(strcmp(str, "?") != 0)
    {
        int event = '0' + atoi(str);

        // write_byte(1,(void *)&event, sizeof(int));
        // // write(1,str, 1);

        // printf("%d\n",event);
        send_msg_all((void *)&event, sizeof(int));
    }
    

}