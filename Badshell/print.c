#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include "common.c"

int main(int argc, char **argv)
{
	int *shm_argv;
	int shm_argc;
	char *shm_out;

	char *arg_str;
	int i, j;
	int str_index;

	FILE *test_file;


	test_file = fopen("test_file.txt", "w");
	if(argc != 4)
	{
		printf("sum: Improper arguments passed\n");
		exit(2);
	}
	shm_argc = atoi(argv[1]);
	shm_argv = (int *)shmat(atoi(argv[2]), NULL, 0);
	shm_out = (char *)shmat(atoi(argv[3]), NULL, 0);
	
	if(shm_argv == NULL || shm_out == NULL)
	{
		printf("sum: Failed to attach shared memory\n");
		exit(2);
	}

	str_index = 0;
	for(i = 0; i < shm_argc; i++)
	{
		arg_str = (char *)shmat(shm_argv[i], NULL, SHM_RDONLY);
		for(j = 0; arg_str[j] != '\0'; j++)
		{
			shm_out[str_index++] = arg_str[j];
		}
		shmdt(arg_str);
		shm_out[str_index++] = '\n';
	}
	shm_out[str_index] = '\0';
	shmdt(shm_argv);
	shmdt(shm_out);
	return 0;
}

