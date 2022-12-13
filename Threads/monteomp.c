#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <omp.h>

#define LENGTH 100000
int main(void)
{
    double *x_axis;
    double *y_axis;
    int i;
    
    int ins_total;

    x_axis = malloc(sizeof(double) * LENGTH);
    y_axis = malloc(sizeof(double) * LENGTH);
    for(i = 0; i < LENGTH; i++)
    {
        x_axis[i] = (double)rand() / RAND_MAX * 2 - 1;
        y_axis[i] = (double)rand() / RAND_MAX * 2 - 1;
    }

    ins_total = 0;

    #pragma omp parallel
    {
        int ins;
        int j;
        int nthreads = omp_get_num_threads();
        int id = omp_get_thread_num();

        for(j = id; j < LENGTH; j += nthreads)
        {
            if(x_axis[j]*x_axis[j] + y_axis[j]*y_axis[j] < 1)
                ins++;
        }
        #pragma omp critical
        {
            ins_total += ins;
        }
    }
    printf("%lf\n", (double)ins_total/LENGTH * 4);

    free(x_axis);
    free(y_axis);
    return 0;
}