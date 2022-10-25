#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
//No synchronized, but...
//A DIYS barrier to start simultaneously, 

#define SHARED 1
pthread_mutex_t lock;


void *producer(void *); 
void *consumer(void *);

int green;
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
     green=0;
     pthread_create(&pid, &attr, producer, NULL);
     pthread_create(&cid, &attr, consumer, NULL);
     pthread_join(pid, NULL);
     pthread_join(cid, NULL);
     pthread_mutex_destroy(&lock) ;
}

void *producer(void *arg) {
     int produced;

           pthread_mutex_lock(&lock) ;
           green++;
           pthread_mutex_unlock(&lock) ;
	   while(green<2);

     for (produced = 1; produced <= numIters; produced++) {
           data = produced;
      }
}

void *consumer(void *arg) {
     int total = 0, consumed;

           pthread_mutex_lock(&lock) ;
           green++;
           pthread_mutex_unlock(&lock) ;
	   while(green<2);

     for (consumed = 1; consumed <= numIters; consumed++) {
           total = total + data;

      }
      printf("Total: %d\n", total);
}

