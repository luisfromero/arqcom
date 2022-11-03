#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
//Active wait

char HW[13]; /* shared buffer */
sem_t green;
void *producer()
{
sleep(5);
strcpy(HW,"Hello World");
sem_post(&green);
}
void *consumer()
{
    while ( sem_trywait(&green))
	{
	printf("Consumer doing some job while waiting\n");
	sleep(1);
	}
    printf("%s\n",HW);
}


main(int argc, char *argv[])
{
int i;
pthread_t pid,cid;
pthread_attr_t attr;
pthread_attr_init(&attr);
sem_init(&green, 0, 0);  //Red
pthread_create(&pid,&attr, producer,NULL);
pthread_create(&cid,&attr, consumer,NULL);

for(i=9;i>=0;i--){
        printf("%d\n",i);
        sleep(1);
        }

pthread_join(pid,NULL);
pthread_join(cid,NULL);
}

