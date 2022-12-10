#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

struct Node
{
	int value;
	int next_shm_id;	
};
struct SharedLinkedList
{
	int head;
};


int create_list(void);
int pop(int list_id);
int get_at_index(int list_id, int index);
void push(int list_id, int value);
void destroy(int list_id);

int main(void)
{
	int list_id;
	int i;
	
	list_id = create_list();

	if(fork() == 0)
		for(i = 0; i < 10; i++)
			push(list_id, i);
	else
	{
		wait(NULL);

		printf("%d\n", pop(list_id));
		printf("%d\n", get_at_index(list_id, 5));
		printf("%d\n", get_at_index(list_id, 4));
		destroy(list_id);
	}
	return 0;
}
/*Creates shared linked list in shared memory, returns id*/
int create_list(void)
{
	struct SharedLinkedList *list;
	int shm_id;

	shm_id = shmget(IPC_PRIVATE, sizeof(struct SharedLinkedList), IPC_CREAT | 0666);

	if(shm_id < 0)
	{
		printf("shmget error\n");
		exit(1);
	}


	list = (struct SharedLinkedList *)shmat(shm_id, NULL, 0);
	list->head = -1;
	shmdt(list);

	return shm_id;
}

void push(int list_id, int value)
{
	struct SharedLinkedList *list;
	struct Node *new;
	int new_id;

	list = (struct SharedLinkedList *)shmat(list_id, NULL, 0);

	new_id = shmget(IPC_PRIVATE, sizeof(struct Node), IPC_CREAT | 0666);

	if(new_id < 0)
	{
		printf("shmget error\n");
		destroy(list_id);
		exit(1);
	}

	new = (struct Node *)shmat(new_id, NULL, 0);
	new->value = value;
	new->next_shm_id = list->head;
	list->head = new_id;
	shmdt(list);
	shmdt(new);


	list = (struct SharedLinkedList *)shmat(list_id, NULL, 0);
	shmdt(list);
}
int pop(int list_id)
{
	int out;
	int old_head_id;
	struct SharedLinkedList *list;
	struct Node *head;

	list = (struct SharedLinkedList * )shmat(list_id, NULL, 0);
	if(list->head == -1)
	{
		printf("Pop called on an empty list\n");
		destroy(list_id);
		exit(1);
	}
	old_head_id = list->head;
	head = (struct Node * )shmat(old_head_id, NULL, 0);

	out = head->value;
	list->head = head->next_shm_id;

	shmdt(head);
	shmctl(old_head_id, IPC_RMID, NULL);
	shmdt(list);

	return out;
}

int get_at_index(int list_id, int index)
{
	int next_shm_id;
	int out;

	struct Node *current_node;
	struct SharedLinkedList *list;
	
	if(index < 0)
	{
		printf("Out of bounds\n");
		destroy(list_id);
		exit(1);
	}
	list = (struct SharedLinkedList * )shmat(list_id, NULL, 0);
	if(list->head == -1)
	{
		printf("Out of bounds\n");
		destroy(list_id);
		exit(1);
	}
	current_node = (struct Node *)shmat(list->head, NULL, 0);
	while(index-- > 0)
	{
		if(current_node->next_shm_id == -1)
		{
			printf("Out of bounds\n");
			destroy(list_id);
			exit(1);
		}
		next_shm_id = current_node->next_shm_id;
		shmdt(current_node);
		current_node = shmat(next_shm_id, NULL, 0);
	}
	out = current_node->value;
	shmdt(current_node);
	return out;
}
void destroy(int list_id)
{
	struct SharedLinkedList *list;

	list = (struct SharedLinkedList * )shmat(list_id, NULL, 0);
	while(list->head != -1)
		pop(list_id);
	
	shmdt(list);
	shmctl(list_id, IPC_RMID, NULL);
}
