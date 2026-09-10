#include <stdlib.h>
#include <stdio.h>

main(int argc, char *argv[])
{
int i;
int j=8;
int q=0;
float b=0;
float k[100000];
for(i=0;i<100000;i++){
	b=b+k[i];
	k[i]=b;
        }
printf("%e\n",b);
}

