#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
//Introducing Mutex 

#define SHARED 1
pthread_mutex_t lock;


void *producer(void *); 
void *consumer(void *);

int green; //protected variable
int data; /* shared buffer */
int numIters;

int main(int argc, char * argv[ ]) {
      pthread_t pid, cid;    
      pthread_attr_t attr;   
      pthread_attr_init(&attr);
      pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

     numIters = atoi(argv[1]);
     printf("Starting...\n");
     pthread_mutex_init(&lock , NULL) ;
     pthread_create(&pid, &attr, producer, NULL);
     pthread_create(&cid, &attr, consumer, NULL);
     pthread_join(pid, NULL);
     pthread_join(cid, NULL);
     pthread_mutex_destroy(&lock) ;
}

void *producer(void *arg) {
     int produced;
     for (produced = 1; produced <= numIters; produced++) {
	   while(green==1);
           data = produced;
           pthread_mutex_lock(&lock) ;
	   green=1;
           pthread_mutex_unlock(&lock) ;
      }
}

void *consumer(void *arg) {
     int total = 0, consumed;
     for (consumed = 1; consumed <= numIters; consumed++) {
	   while(green==0);
           total = total + data;
           pthread_mutex_lock(&lock) ;
	   green=0;
           pthread_mutex_unlock(&lock) ;

      }
      printf("Total: %d\n", total);
}

