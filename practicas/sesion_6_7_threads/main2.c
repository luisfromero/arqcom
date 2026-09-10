#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
//Producer and consumer


char HW[12]; /* shared buffer */

void *producer()
{
sleep(5);
printf("Copying message\n");
strcpy(HW,"Hello World");
}

void *consumer()
{
sleep(3);
printf("%s\n",HW);
sleep(4);
printf("%s\n",HW);
}

main(int argc, char *argv[])
{
int i;
pthread_t pid,cid;
strcpy(HW,"rgjtserlky");
pthread_create(&pid,NULL, producer,NULL);
pthread_create(&cid,NULL, consumer,NULL);
for(i=9;i>=0;i--){
        printf("%d\n",i);
        sleep(1);
        }
pthread_join(pid,NULL);
pthread_join(cid,NULL);
}

