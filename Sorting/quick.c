#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/time.h>

void quicksort_fork(int shm_id, int low, int high, int depth);
void quicksort_vanilla(int *arr, int low, int high);

int main(int argc, char **argv)
{
	struct timeval stop, start;
	int badcount;
	int shm_id;
	int i;
	int max;
	int *shm_ptr;
	int *nums;

	if(argc < 2)
	{
        	printf("No number given\n");
        	exit(1);
    	}
	i = 0;
	max = 0;
    	while(argv[1][i] >= '0' && argv[1][i] <= '9')
    	{
        	max = max * 10 + (argv[1][i] - '0');
        	i++;
    	}

	nums = malloc(sizeof(int) * max);
	for(i = 0; i < max; i++)
		nums[i] = rand();

	shm_id = shmget(IPC_PRIVATE, sizeof(int)*max, IPC_CREAT | 0666);
	shm_ptr = (int * ) shmat(shm_id, NULL, 0);

	for(i = 0; i < max; i++)
		shm_ptr[i] = nums[i];

	printf("Beginning quick and forky sort\n");
	gettimeofday(&start, NULL);
	quicksort_fork(shm_id, 0, max-1, 0);
	gettimeofday(&stop, NULL);
	
	printf("Time spent: %f\n", 	(stop.tv_sec - start.tv_sec) + (double)(stop.tv_usec - start.tv_usec) / 1000000);

	badcount = 0;
	for(i = 0; i < max-1; i++)
		if(shm_ptr[i+1] < shm_ptr[i])
			badcount++;

	printf("Unsorted items: %d\n\n", badcount);

	for(i = 0; i < max; i++)
		shm_ptr[i] = nums[i];

	printf("Beginning regular quicksort\n");
	gettimeofday(&start, NULL);
	quicksort_vanilla(shm_ptr, 0, max-1);
	gettimeofday(&stop, NULL);
	printf("Time spent: %f\n", (stop.tv_sec - start.tv_sec) + (double)(stop.tv_usec - start.tv_usec) / 1000000);
	
    badcount = 0;
	for(i = 0; i < max-1; i++)
		if(shm_ptr[i+1] < shm_ptr[i])
			badcount++;

	printf("Unsorted items: %d\n\n", badcount);

	shmdt(shm_ptr);
	shmctl(shm_id, IPC_RMID, NULL);
	return 0;
}

void quicksort_fork(int shm_id, int low, int high, int depth)
{
	int i, j;
	int pivot, temp;
	int *arr;

	arr = (int * ) shmat(shm_id, NULL, 0);
	if(low >= high)
		return;

	pivot = arr[high];
	for(j = i = low; j <= high - 1; j++)
	{
		if(arr[j] < pivot)
		{
			temp = arr[i];
			arr[i] = arr[j];
			arr[j] = temp;
			i++;
		}
	}
	
	temp = arr[i];
	arr[i] = arr[j];
	arr[j] = temp;

	if(depth < 8)
	{
		shmdt(arr);
		if(fork() == 0)
		{
			quicksort_fork(shm_id, i+1, high, depth+1);
			exit(0);
		}
		else
		{
			quicksort_fork(shm_id, low, i-1, depth+1);
			wait(NULL);

		}
	}
	else
	{
		quicksort_vanilla(arr, low, i-1);
		quicksort_vanilla(arr, i+1, high);
		shmdt(arr);
	}
}

void quicksort_vanilla(int *arr, int low, int high)
{
	int i, j;
	int pivot, temp;
	if(low >= high)
		return;
	pivot = arr[high];
	for(j = i = low; j <= high - 1; j++)
	{
		if(arr[j] < pivot)
		{
			temp = arr[i];
			arr[i] = arr[j];
			arr[j] = temp;
			i++;
		}
	}
	temp = arr[i];
	arr[i] = arr[j];
	arr[j] = temp;
	quicksort_vanilla(arr, low, i-1);
	quicksort_vanilla(arr, i+1, high);
}
