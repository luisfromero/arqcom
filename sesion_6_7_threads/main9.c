#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#define SHARED 0

//Experiment to compare semaphores and mutex

sem_t  green;
pthread_mutex_t lock;
int shared_data;

void *thread_func(void *arg)
{
int i;
for(i=0; i<300*1024*1024; ++i)
{
//pthread_mutex_lock(&lock) ;
sem_wait(&green); //add one, usually 1 to 2 
shared_data++;
sem_post(&green); //sub one
//pthread_mutex_unlock(&lock) ;
}
return NULL;
}





int main (void)
{
pthread_t th;
void *exitstatus;
int i;
sem_init(&green, SHARED, 1);



pthread_mutex_init(&lock , NULL) ;
pthread_create(&th, NULL, thread_func , NULL) ;

for ( i = 0 ; i < 10 ; ++i ) {
sleep(1);
//pthread_mutex_lock(&lock) ;
sem_wait(&green);
printf("\rValue = %d\n" , shared_data);
sem_post(&green);
//pthread_mutex_unlock(&lock) ;

}
printf("\n");
pthread_join(th, &exitstatus);
pthread_mutex_destroy(&lock);
return 0 ;
}
