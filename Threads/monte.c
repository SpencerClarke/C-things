#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <math.h>
#include <pthread.h>

#define THREADS 100
#define POINTS_PER_THREAD 1000000
struct thread_data
{
    double *x_arr;
    double *y_arr;
    int len;
};
void *monte(void *arg);

int sum;
pthread_mutex_t lock;

int main(void)
{
    pthread_t * tids;
    struct thread_data **data;
    double pi;
    int i;
    int j;

    tids = malloc(sizeof(pthread_t) * THREADS);
    data = malloc(sizeof(struct thread_data * ) * THREADS * POINTS_PER_THREAD);

    sum = 0;
    for(i = 0; i < THREADS; i++)
    {
        data[i] = malloc(sizeof(struct thread_data));
        data[i]->x_arr = malloc(sizeof(double) * POINTS_PER_THREAD);
        data[i]->y_arr = malloc(sizeof(double) * POINTS_PER_THREAD);
        for(j = 0; j < POINTS_PER_THREAD; j++)
        {
            data[i]->x_arr[j] = ((double)rand()/RAND_MAX) * 2 - 1;
            data[i]->y_arr[j] = ((double)rand()/RAND_MAX) * 2 - 1;
            data[i]->len = POINTS_PER_THREAD;
        }
        pthread_create(&tids[i], NULL, monte, (void * )(data[i]));
    }
    for(i = 0; i < THREADS; i++)
    {
        pthread_join(tids[i], NULL);
        free(data[i]);
    }
    free(data);
    free(tids);
    pi = (double)sum / (POINTS_PER_THREAD*(THREADS))*4;

    printf("%lf\n", pi);
    return 0;
}
void *monte(void *arg)
{
    struct thread_data *data;
    int i;
    int in_count;
    int total_count;

    data = (struct thread_data * )arg;
    in_count = total_count = 0;
    for(i = 0; i < data->len; i++)
    {
        if(sqrt(data->x_arr[i]*data->x_arr[i] + data->y_arr[i]*data->y_arr[i]) < 1.0)
        {
            in_count++;
        }
        total_count++;
    }

    pthread_mutex_lock(&lock);
    sum += in_count;
    pthread_mutex_unlock(&lock);
    pthread_exit(NULL);
}