#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include "common/shark.h"
#include "common/timing.h"
unsigned short int m[dimy][dimx];

// version 2
// 4 threads: 4 workers
// Added OMP FOR 
// Speedup 5.7

#define NUM_THREADS 4
thread_data td[NUM_THREADS];



int usex=0;

void life(unsigned short int m[][dimx])
{
int id, np;
omp_set_num_threads(NUM_THREADS);
#pragma omp parallel default(shared) private(id,np)
{
int ll,lh;
id=omp_get_thread_num();
np=omp_get_num_threads();
td[id].nt=NUM_THREADS;
td[id].id=id;
#pragma omp ordered
{
printf("Thread %d of %d \n",id,np);
}

for(int t=0;t<1000;t++)
{
	int stpp=t%2;
	#pragma omp for schedule(guided,10)
	for(int i=0;i<dimy;i++)
		for(int j=0;j<dimx;j++)
			item_(m,&i,&j,&stpp,&td[id]);
}
#pragma omp critical
{
printf("Thread %d of %d  with %d sharks\n",id,np,td[id].nshark);
}
}
}

main()
{
hrtime starttime,endtime;
initialize_(m,usex,0,(thread_data *)&td);
starttime=gethrtime();
life(m);
endtime=gethrtime();
printf("Time: %f GigaTicks, (%f sec.)\n",(endtime-starttime)/1.0e9, (endtime-starttime)/2.666e9);
finalize_(usex);
}
