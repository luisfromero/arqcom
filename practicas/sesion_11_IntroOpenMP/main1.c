#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include "common/shark.h"
#include "common/timing.h"

// Version 1:
//
// 3 Workers, One painter
// Hand-made Partitions
// Barrier & Critical directives
// Results: Speedup 4.1

unsigned short int m[dimy][dimx];
#define NUM_THREADS 4
thread_data td[NUM_THREADS];
int usex=1;

void life(unsigned short int m[][dimx])
{
hrtime starttime,endtime;
int id, np;
omp_set_num_threads(NUM_THREADS);
int counter=0;
#pragma omp parallel default(shared) private(id,np)
{
int ll,lh;
id=omp_get_thread_num();
np=omp_get_num_threads();
//#pragma omp barrier
//#pragma omp critical
{
printf("%d of %d \n",id,np);
}
#pragma omp barrier
starttime=gethrtime();
if(id==0){
	ll=0;
	lh=199;
}
if(id==1){
	ll=200;
	lh=399;
}
if(id==2){
	ll=400;
	lh=599;
}
if(id<3)

for(int t=0;t<1000;t++)
{

int stpp=t%2;
for(int i=ll;i<=lh;i++)
	for(int j=0;j<dimx;j++)
		item_(m,&i,&j,&stpp,&td[id]);
}



#pragma omp critical
{
	counter++;
}
if(id==3)

	do{

	drawWorld_(m,usex);
}
	while(counter<4);
endtime=gethrtime();
printf("%d of %d with %d sharks in %f GigaTicks\n",id,np,td[id].nshark,(endtime-starttime)/1.0e9);
}
}

main()
{
td[0].nt=NUM_THREADS;
initialize_(m,usex,0,(thread_data *)&td);
life(m);
finalize_(usex);
}
