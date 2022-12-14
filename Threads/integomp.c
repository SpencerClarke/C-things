#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <omp.h>

#define NUM_THREADS 20

int main(int argc, char **argv)
{
    double sum;
    int size;
    double step;

	if(argc < 2)
	{
		printf("Specify number of rectangles\n");
		exit(2);
	}
	size = atoi(argv[1]);
    step = 1.0/size;
	sum = 0;    

    #pragma omp parallel
    {
        int i;
        int tid;
        int tcount;
        double area;
        double x;
        
        tid = omp_get_thread_num();
        tcount = omp_get_num_threads();
        for(i = tid; i < size; i += tcount)
        {
            x = (i+0.5)*step;
            area += (4/(1+x*x));
        }
        area *= step;
        #pragma omp critical
        {
            sum += area;
        }
    }
	printf("%lf\n", sum);
	return 0;
}

