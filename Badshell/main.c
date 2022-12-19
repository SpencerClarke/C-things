#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "command_trie.c"
#include "common.c"

int main(void)
{
	int *shm_arg_ids;

	int *shm_argv;
	char *shm_arg;

	int shm_out_id;
	char *shm_out_id_str;
	char *shm_out;

	int shm_argc;
	char *shm_argc_str;

	int shm_argv_id;
	char *shm_argv_id_str;

	char *buffer;
	int buff_len;
	int c;
	int end;

	int i;

	pid_t pid;
	int child_status;

	struct Trie command_trie;
	char *command;
	
	/*Create a trie mapping keywords to program calls*/

	command_trie = create_trie();
	add_key(&command_trie, "sum", "./sum.out");
	add_key(&command_trie, "printlines", "./print.out");

	while(1)
	{
		/*Get command name*/
		buffer = malloc(sizeof(char) * COMMAND_NAME_LENGTH+1);
		buff_len = 0;
		printf("> ");
		for(buff_len = 0; (c = getchar()) != ' ' && c != '\n' && buff_len < COMMAND_NAME_LENGTH; buff_len++)
		{
			buffer[buff_len] = c;
		}
		buffer[buff_len] = '\0';
		buff_len = 0;
		if(has_key(&command_trie, buffer))
		{
			command = get_command(&command_trie, buffer);
		}
		else
		{
			if(strcmp(buffer, "exit") == 0)
			{
				puts("Goodbye.");
				exit(0);
			}
			puts("Command not found.");
			continue;
		}

		/*Dynamically write command arguments into shared memory*/
		shm_arg_ids = (int *)malloc(sizeof(int));
		shm_argc = buff_len = 0;
		while(c = getchar())
		{
			if(c == ' ' || (end = (c == '\n' || buff_len >= COMMAND_ARGUMENT_LENGTH)))
			{
				if(buff_len == 0)
					break;

				buffer[buff_len] = '\0';
				shm_arg_ids = (int *)realloc(shm_arg_ids, sizeof(int)*(shm_argc + 1));
				shm_arg_ids[shm_argc] = shmget(IPC_PRIVATE, sizeof(char)*buff_len, IPC_CREAT | 0666);

				shm_arg= (char *)shmat(shm_arg_ids[shm_argc], NULL, 0);
				strcpy(shm_arg, buffer);
				buff_len = 0;
				shm_argc++;
				shmdt(shm_arg);

				if(end || shm_argc >= MAX_COMMAND_COUNT)
					break;	
			}
			else
			{
				buffer[buff_len] = c;
				buff_len++;
			}
		}
		/*Dynamically create an array of shared memory IDs to the aformentioned*/
		/*arguments and put the array in shared memory*/
		if(shm_argc == 0)
			shm_argv_id = -1;
		else
			shm_argv_id = shmget(IPC_PRIVATE, sizeof(int)*shm_argc, IPC_CREAT | 0666);
		shm_argv = (int *)shmat(shm_argv_id, NULL, 0);
		for(i = 0; i < shm_argc; i++)
		{
			shm_argv[i] = shm_arg_ids[i];
		}
		free(shm_arg_ids);
		
		/*Create a large string for storing the called program's output*/
		shm_out_id = shmget(IPC_PRIVATE, sizeof(char) * SHM_OUT_SIZE, IPC_CREAT | 0666);

		/*Call the external program in a new process*/
		if((pid = fork()) == 0)
		{		
			shm_argc_str = malloc(sizeof(char) * SHM_ARGC_SIZE);
			shm_out_id_str = malloc(sizeof(char) * SHM_ID_SIZE);
			shm_argv_id_str = malloc(sizeof(char) * SHM_ID_SIZE);

			int_to_str(shm_argc_str, shm_argc);
			int_to_str(shm_out_id_str, shm_out_id);
			int_to_str(shm_argv_id_str, shm_argv_id);

			execlp(command, command, shm_argc_str, shm_argv_id_str, shm_out_id_str, NULL);

			exit(0);
		}
		else
		{
			waitpid(pid, &child_status, 0);
			shm_out = shmat(shm_out_id, NULL, SHM_RDONLY);
			puts(shm_out);

			shmdt(shm_out);
		}
		
		/*destroy shared memory segments*/
		for(i = 0; i < shm_argc; i++)
		{
			shm_arg = (char *)shmat(shm_argv[i], NULL, 0);
			shmdt(shm_arg);
			shmctl(shm_argv[i], IPC_RMID, NULL);
		}
		shmctl(shm_argv_id, IPC_RMID, NULL);
		shmctl(shm_out_id, IPC_RMID, NULL);
	}
	return 0;
}

