#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

// RAW and WAR hazards. 
// Comment line sem_wait in producer to test

#define SHARED 0
sem_t empty, full; //Buffer status 
int data; /* Shared buffer */
int numIters;


void *producer(void *arg) {
     int produced;
     for (produced = 1; produced <= numIters; produced++) {
           sem_wait(&empty);
           data = produced;
           sem_post(&full); }
}

void *consumer(void *arg) {
int total = 0, consumed;
     for (consumed = 1; consumed <= numIters; consumed++) {
           sem_wait(&full);
           total = total + data;
           sem_post(&empty); 
		}
printf("Total: %d\n", total);
}


int main(int argc, char * argv[ ]) {
      pthread_t pid, cid;  
      pthread_attr_t attr;  
      pthread_attr_init(&attr);
      pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

      sem_init(&empty, SHARED, 1); /* sem empty = 1 */
      sem_init(&full, SHARED, 0);      /* sem full = 0 */
      numIters = atoi(argv[1]);
      printf("Starting...\n");
     	pthread_create(&cid, &attr, consumer, NULL);
     	pthread_create(&pid, &attr, producer, NULL);
     	pthread_join(pid, NULL);
     	pthread_join(cid, NULL);
}


