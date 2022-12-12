#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>

struct thread_data 
{
	int l;
	int r;
	int depth;
};
void *thread_func(void *arg);
void quicksort_global(int l, int r);
void quicksort_vanilla(int l, int r, int *vals);
void *quicksort_threaded(void *arg);
int *arr;
#define SIZE 100000000
int main(void)
{
	int i;

	pthread_t tid;
	struct thread_data *data;
	struct timeval start, stop;

	int *arr2;
	int bad_count;

	arr = malloc(sizeof(int) * SIZE);
	arr2 = malloc(sizeof(int) * SIZE);
	data = malloc(sizeof(struct thread_data));
	for(i = 0; i < SIZE; i++)
	{
		arr[i] = rand();
		arr2[i] = arr[i];
	}
	printf("Finished building\n");

	data->l = 0;
	data->r = SIZE-1;
	data->depth = 0;

	gettimeofday(&start, NULL);
	pthread_create(&tid, NULL, quicksort_threaded, (void *)data);
	pthread_join(tid, NULL);
	gettimeofday(&stop, NULL);
	printf("Time spent on threaded quicksort: %f\n", (stop.tv_sec - start.tv_sec) + (double)(stop.tv_usec - start.tv_usec) / 1000000);

	bad_count = 0;
	for(i = 0; i < SIZE-1; i++)
		if(arr[i] > arr[i+1])
			bad_count++;

	printf("Unsorted elements %d\n", bad_count);
	bad_count = 0;
	gettimeofday(&start, NULL);
	quicksort_vanilla(0, SIZE-1, arr2);
	gettimeofday(&stop, NULL);
	printf("Time spent on normal quicksort: %f\n", (stop.tv_sec - start.tv_sec) + (double)(stop.tv_usec - start.tv_usec) / 1000000);

	bad_count = 0;
	for(i = 0; i < SIZE-1; i++)
		if(arr2[i] > arr2[i+1])
			bad_count++;
	printf("Unsorted elements %d\n", bad_count);
	free(arr);
	free(arr2);
	free(data);
	return 0;
}

void *quicksort_threaded(void *arg)
{
	int i;
	int j;
	
	struct thread_data *data;

	int pivot;
	int temp;
	
	struct thread_data *left_data;
	struct thread_data *right_data;

	pthread_t thread_left;
	pthread_t thread_right;

	data = (struct thread_data *)arg;

	if(data->r <= data->l)
		pthread_exit(NULL);
	pivot = arr[data->r];
	i = data->l;
	for(j = data->l; j < data->r; j++)
	{
		if(arr[j] < pivot)
		{
			temp = arr[j];
			arr[j] = arr[i];
			arr[i] = temp;
			i++;
		}
	}
	temp = arr[i];
	arr[i] = arr[data->r];
	arr[data->r] = temp;
	if(data->depth > 13)
	{
		quicksort_global(data->l, i-1);
		quicksort_global(i+1, data->r);
	}
	else
	{
		left_data = malloc(sizeof(struct thread_data));
		right_data = malloc(sizeof(struct thread_data));
		left_data->l = data->l;
		left_data->r = i-1;
		left_data->depth = data->depth+1;
		right_data->l = i+1;
		right_data->r = data->r;
		right_data->depth = data->depth+1;
		pthread_create(&thread_left, NULL, quicksort_threaded, (void*)left_data);
		pthread_create(&thread_right, NULL, quicksort_threaded, (void*)right_data);
		pthread_join(thread_left, NULL);
		pthread_join(thread_right, NULL);
		free(left_data);
		free(right_data);
	}
	pthread_exit(NULL);
}

void quicksort_global(int l, int r)
{
	int i;
	int j;

	int pivot;
	int temp;

	if(r <= l)
		return;
	
	pivot = arr[r];
	i = l;
	for(j = l; j < r; j++)
	{
		if(arr[j] < pivot)
		{
			temp = arr[j];
			arr[j] = arr[i];
			arr[i] = temp;
			i++;
		}
	}
	temp = arr[i];
	arr[i] = arr[r];
	arr[r] = temp;
	quicksort_global(l, i-1);
	quicksort_global(i+1, r);
}
void quicksort_vanilla(int l, int r, int *vals)
{
	int i;
	int j;

	int pivot;
	int temp;

	if(r <= l)
		return;
	
	pivot = vals[r];
	i = l;
	for(j = l; j < r; j++)
	{
		if(vals[j] < pivot)
		{
			temp = vals[j];
			vals[j] = vals[i];
			vals[i] = temp;
			i++;
		}
	}
	temp = vals[i];
	vals[i] = vals[r];
	vals[r] = temp;
	quicksort_vanilla(l, i-1, vals);
	quicksort_vanilla(i+1, r, vals);
}
