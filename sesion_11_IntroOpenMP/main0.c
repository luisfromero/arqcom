#include <stdlib.h>
#include <stdio.h>
#include "common/shark.h"
#include "common/timing.h"

unsigned short int m[dimy][dimx];
thread_data td;



void life(unsigned short int m[][dimx])
{
hrtime starttime,endtime;
starttime=gethrtime();
for(int t=0;t<1000;t++)
	{
	int stpp=t%2;
	for(int i=0;i<dimy;i++)
		for(int j=0;j<dimx;j++)
			item_(m,&i,&j,&stpp,&td);
	drawWorld_(m,1);
	}
endtime=gethrtime();
double time=(endtime-starttime)/1.0e9;
printf("Time: %f GigaTicks, (%f sec.)\n",(endtime-starttime)/1.0e9, (endtime-starttime)/2.666e9);
}

main()
{
initialize_(m,1,0,&td);
life(m);
finalize_(1);
}
