#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define NUM_THREADS 30

void *integ(void *arg);

double sum;
int size;
pthread_mutex_t lock;

int main(int argc, char **argv)
{
	int i;
	int *offsets;
	pthread_t *tids;

	if(argc < 2)
	{
		printf("Specify number of rectangles\n");
		exit(2);
	}
	size = atoi(argv[1]);
	sum = 0;

	offsets = malloc(sizeof(int * ) * NUM_THREADS);
	tids = malloc(sizeof(pthread_t * ) * NUM_THREADS);
	for(i = 0; i < NUM_THREADS; i++)
		offsets[i] = i;
	for(i = 0; i < NUM_THREADS; i++)
		pthread_create(&tids[i], NULL, integ, (void * )(&offsets[i]));
	for(i = 0; i < NUM_THREADS; i++)
		pthread_join(tids[i], NULL);

	printf("%lf\n", sum);
	return 0;
}

void *integ(void *arg)
{
	int *offset;
	int i;

	double area;
	double step;
	double x;

	offset = (int *)arg;
	
	step = 1.0/size;
	for(i = *offset; i < size; i += NUM_THREADS)
	{
		x = i*step;
		area += (4/(1+x*x));
	}
	
	area *= step;
	pthread_mutex_lock(&lock);
	sum += area;
	pthread_mutex_unlock(&lock);
	pthread_exit(NULL);
}
