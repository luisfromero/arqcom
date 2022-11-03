#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
//Only a thread

void *thread_func(void* argv)
{
sleep(5);
printf("Hello world\n");
}

main(int argc, char *argv[])
{
int i;
pthread_t mythread,my2;    /* thread and attributes */
pthread_create(&mythread,NULL, thread_func,NULL);
pthread_create(&my2,NULL, thread_func,NULL);
for(i=9;i>=0;i--){
        printf("%d\n",i);
        sleep(1);
        }
pthread_join(mythread,NULL);
pthread_join(my2,NULL);
}

