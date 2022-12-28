`#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>
#include <time.h>
#include <unistd.h>

#define BUFF_SIZE 4

struct TestNode 
{
	struct TestNode *next;
	struct TestNode *prev;

	wchar_t *meaning;
	wchar_t *writing;
	wchar_t *reading;
};
struct TestList
{
	struct TestNode *head;
	struct TestNode *tail;
	struct TestNode *focus;

	int len;
};

struct RecallNode
{
	struct RecallNode *next;
	int correct;
};
struct RecallStack
{
	struct RecallNode *head;

	int len;
};

struct RecallStack recall_create(void);
void recall_push(struct RecallStack *stack, int correct);
int recall_pop(struct RecallStack *stack);
int recall_empty(struct RecallStack *stack);
void recall_destroy(struct RecallStack *stack);

struct TestList test_create(void);
void test_rand_insert(struct TestList *list, wchar_t *meaning, wchar_t *reading, wchar_t *writing);
int test_submit(struct TestList *list, wchar_t *answer);
wchar_t *test_peek_reading(struct TestList *list);
wchar_t *test_peek_writing(struct TestList *list);
wchar_t *test_peek_meaning(struct TestList *list);
int test_revert(struct TestList *list, struct RecallStack *stack, int n);
void test_destroy(struct TestList *list);

int main(int argc, char **argv)
{
	int i;
	wchar_t c;
	int ac;
	int is_num;
	int ret;
	int state;

	int reading_buffer_size;
	wchar_t *reading_buffer;
	wchar_t *current_reading;

	int writing_buffer_size;
	wchar_t *writing_buffer;
	wchar_t *current_writing;

	int meaning_buffer_size;
	wchar_t *meaning_buffer;
	wchar_t *current_meaning;

	int input_buffer_size;
	wchar_t *input_buffer;

	int pos;
	struct TestList list;
	struct RecallStack stack;

	FILE *fin;

	if(argc < 2)
	{
		puts("No filename given");
		exit(2);
	}
	fin = fopen(argv[1], "r");
	if(fin == NULL)
	{
		puts("File not found");
		exit(2);
	}

	setlocale(LC_CTYPE,"C.UTF-8");

	list = test_create();

	meaning_buffer_size = reading_buffer_size = writing_buffer_size = BUFF_SIZE;
	
	reading_buffer = (wchar_t * )malloc(sizeof(wchar_t) * reading_buffer_size);
	if(reading_buffer == NULL)
	{
		perror("Malloc error");
		exit(2);
	}
	writing_buffer = (wchar_t * )malloc(sizeof(wchar_t) * writing_buffer_size);
	if(writing_buffer == NULL)
	{
		perror("Malloc error");
		exit(2);
	}
	meaning_buffer = (wchar_t * )malloc(sizeof(wchar_t) * meaning_buffer_size);
	if(meaning_buffer == NULL)
	{
		perror("Malloc error");
		exit(2);
	}

	srand(time(NULL));

	state = 0;
	pos = 0;
	while((c = getwc(fin)))
	{
		if(state == 0)
		{
			if(pos == 0)
			{
				if(c == '\n' || c == ' ')
					continue;
				if(c == WEOF)
					break;
			}
			if(pos >= meaning_buffer_size-1)
			{
				meaning_buffer_size *= 2;
				meaning_buffer = (wchar_t *)realloc(meaning_buffer, sizeof(wchar_t) * meaning_buffer_size);
				if(meaning_buffer == NULL)
				{
					perror("Realloc error");
					exit(3);
				}
			}
			if(c == ',')
			{
				meaning_buffer[pos] = '\0';
				state = 1;
				pos = 0;
			}
			else if(c == WEOF || c == '\n')
			{
				wprintf(L"File improperly formatted.\n");

				free(reading_buffer);
				free(writing_buffer);
				free(meaning_buffer);
				fclose(fin);
				test_destroy(&list);

				exit(1);
			}
			else
			{
				meaning_buffer[pos] = c;
				pos++;
			}

		}
		else if(state == 1)
		{
			if(pos >= reading_buffer_size-1)
			{
				reading_buffer_size *= 2;
				reading_buffer = (wchar_t *)realloc(reading_buffer, sizeof(wchar_t) * reading_buffer_size);
				if(reading_buffer == NULL)
				{
					perror("Realloc error");
					exit(3);
				}
			}
			if(c == ',')
			{
				reading_buffer[pos] = '\0';
				state = 2;
				pos = 0;
			}
			else if(c == WEOF)
			{
				reading_buffer[pos] = '\0';
				test_rand_insert(&list, meaning_buffer, reading_buffer, reading_buffer);
				break;
			}
			else if(c == '\n')
			{
				reading_buffer[pos] = '\0';
				test_rand_insert(&list, meaning_buffer, reading_buffer, reading_buffer);
				state = 0;
				pos = 0;
			}
			else
			{
				reading_buffer[pos] = c;
				pos++;
			}
		}
		else if(state == 2)
		{
			if(pos >= writing_buffer_size-1)
			{
				writing_buffer_size *= 2;
				writing_buffer = (wchar_t *)realloc(writing_buffer, sizeof(wchar_t) * writing_buffer_size);
				if(writing_buffer == NULL)
				{
					perror("Realloc error");
					exit(3);
				}
			}
			if(c == WEOF)
			{
				writing_buffer[pos] = '\0';
				test_rand_insert(&list, meaning_buffer, reading_buffer, writing_buffer);
				break;
			}
			else if(c == '\n')
			{
				writing_buffer[pos] = '\0';
				test_rand_insert(&list, meaning_buffer, reading_buffer, writing_buffer);
				state = 0;
				pos = 0;
			}
			else
			{
				writing_buffer[pos] = c;
				pos++;
			}
		}	
	}
	fclose(fin);
	free(reading_buffer);
	free(writing_buffer);
	free(meaning_buffer);

	stack = recall_create();

	input_buffer_size = meaning_buffer_size > reading_buffer_size ? meaning_buffer_size : reading_buffer_size;
	input_buffer_size = input_buffer_size > writing_buffer_size ? input_buffer_size : writing_buffer_size;
	input_buffer = (wchar_t *)malloc(sizeof(wchar_t) * input_buffer_size);
	if(input_buffer == NULL)
	{
		perror("Malloc error");
		exit(2);
	}

	system("clear");
	wprintf(L"Terms remaining: %d\n\n\n", list.len);
	ret = 0;
	while(ret != 2)
	{
		current_meaning = test_peek_meaning(&list);
		current_reading = test_peek_reading(&list);
		current_writing = test_peek_writing(&list);

		wprintf(L"%ls: ", current_meaning);
		is_num = 1;
		ac = 0;
		for(i = 0;(c = getwchar()) != '\n' && i < input_buffer_size-1; i++)
		{
			if(c <= '9' && c >= '0')
				ac = ac * 10 + c - '0';
			else
				is_num = 0;
			input_buffer[i] = c;
		}
		input_buffer[i] = '\0';

		system("clear");

		if(i == 0)
			is_num = 0;
		
		if(is_num)
		{
			if(test_revert(&list, &stack, ac) == 1)
			{
				wprintf(L"Going back %d questions.\nTerms remaining: %d\n", ac, list.len); 
			}
			else
			{

				wprintf(L"Could not go back %d questions.\nTerms remaining: %d\n", ac, list.len); 
			}
		}
		else
		{
			ret = test_submit(&list, input_buffer);
			recall_push(&stack, ret);
			if(ret)
				wprintf(L"Correct.\nTerms remaining: %d\n", list.len);
			else
				wprintf(L"Incorrect.\nMeaning: %ls\nReading: %ls\nWriting: %ls\n\nTerms remaining: %d\n", current_meaning, current_reading, current_writing, list.len);
		}
		wprintf(L"\n\n\n");
	}
	
	free(input_buffer);
	recall_destroy(&stack);
	test_destroy(&list);

	system("clear");
	wprintf(L"Set complete. Congrats.\n");

	return 0;
}

struct RecallStack recall_create(void)
{
	struct RecallStack stack;

	stack.head = NULL;
	stack.len = 0;

	return stack;
}
int recall_empty(struct RecallStack *stack)
{
	return stack->head == NULL;
}
void recall_push(struct RecallStack *stack, int correct)
{
	struct RecallNode *new;

	new = (struct RecallNode *)malloc(sizeof(struct RecallNode));
	if(new == NULL)
	{
		perror("Malloc error");
		exit(1);
	}
	new->correct = correct;

	new->next = stack->head;
	stack->head = new;
	stack->len++;
}
int recall_pop(struct RecallStack *stack)
{
	struct RecallNode *temp;
	int correct;

	if(recall_empty(stack))
	{
		perror("Pop called on empty recall stack");
		exit(1);
	}
	temp = stack->head;
	correct = temp->correct;

	stack->head = stack->head->next;
	free(temp);

	return correct;
}
void recall_destroy(struct RecallStack *stack)
{
	while(!recall_empty(stack))
	{
		recall_pop(stack);
	}
}

struct TestList test_create(void)
{
	struct TestList list;

	list.head = NULL;
	list.tail = NULL;
	list.focus = NULL;
	list.len = 0;

	return list;
}
void test_rand_insert(struct TestList *list, wchar_t *meaning, wchar_t *reading, wchar_t *writing)
{
	struct TestNode *new;
	struct TestNode *current;
	int pos;
	int i;

	new = (struct TestNode *)malloc(sizeof(struct TestNode));
	if(new == NULL)
	{
		perror("Malloc error");
		exit(1);
	}

	new->meaning = (wchar_t *)malloc(sizeof(wchar_t) * (wcslen(meaning) + 1));
	if(new->meaning == NULL)
	{
		perror("Malloc error");
		exit(1);
	}
	wcscpy(new->meaning, meaning);

	new->reading = (wchar_t *)malloc(sizeof(wchar_t) * (wcslen(reading)+1));
	if(new->reading == NULL)
	{
		perror("Malloc error");
		exit(1);
	}
	wcscpy(new->reading, reading);

	new->writing = (wchar_t *)malloc(sizeof(wchar_t) * (wcslen(writing)+1));
	if(new->writing == NULL)
	{
		perror("Malloc error");
		exit(1);
	}
	wcscpy(new->writing, writing);

	current = list->head;
	pos = rand() % (list->len + 1);
	if(current == NULL)
	{
		new->next = NULL;
		new->prev = NULL;
		list->head = new;
		list->tail = new;
		list->focus = new;
	}
	else if(pos == 0)
	{
		new->next = list->head;
		list->head->prev = new;
		list->head = new;
		new->prev = NULL;
		list->focus = new;
	}
	else if(pos == list->len)
	{
		new->next = NULL;
		new->prev = list->tail;
		list->tail->next = new;
		list->tail = new;
	}
	else
	{
		for(i = 1; i < pos; i++)
		{
			current = current->next;
		}
		new->next = current->next;
		current->next = new;
		new->prev = current;
		if(new->next != NULL)
			new->next->prev = new;
	}
	list->len++;
}
int test_submit(struct TestList *list, wchar_t *answer)
{
	struct TestNode *temp;
	if(list->len == 0)
	{
		perror("Submitted answer on finished set");
		exit(2);
	}
	if(wcscmp(list->focus->writing, answer) == 0)
	{
		if(list->focus->next == NULL)
		{
			return 2;
		}
		list->focus = list->focus->next;
		list->len--;
		return 1;
	}
	else
	{
		if(list->len == 1)
		{
			return 0;
		}
		temp = list->focus;
		list->focus = list->focus->next;
		if(temp->prev == NULL)
		{
			list->head = temp->next;
		}
		else
		{
			temp->prev->next = temp->next;
		}
		temp->next->prev = temp->prev;
		temp->next = NULL;
		temp->prev = list->tail;
		list->tail->next = temp;
		list->tail = temp;
		return 0;
	}
}

wchar_t *test_peek_meaning(struct TestList *list)
{
	if(list->len == 0)
	{
		perror("Peeked meaning when all questions answered");
		exit(1);
	}
	return list->focus->meaning;
}
wchar_t *test_peek_reading(struct TestList *list)
{
	if(list->len == 0)
	{
		perror("Peeked reading when all questions answered");
		exit(1);
	}
	return list->focus->reading;
}
wchar_t *test_peek_writing(struct TestList *list)
{
	if(list->len == 0)
	{
		perror("Peeked writing when all questions answered");
		exit(1);
	}
	return list->focus->writing;
}
int test_revert(struct TestList *list, struct RecallStack *stack, int n)
{
	int i;
	int cval;
	struct TestNode *temp;

	if(n > stack->len)
	{
		return 0;
	}
	for(i = 0; i < n; i++)
	{
		cval = recall_pop(stack);
		stack->len--;
		if(cval == 1)
		{
			list->focus = list->focus->prev;
			list->len++;	
		}
		else
		{
			if(list->focus->next == NULL)
				continue;
			
			temp = list->tail->prev;
			
			list->tail->prev = list->focus->prev;
			list->tail->next = list->focus;
			list->focus->prev = list->tail;
			temp->next = NULL;
			list->focus = list->tail;

			if(list->focus->prev == NULL)
			{
				list->head = list->focus;
			}
			else
			{
				list->tail->prev->next = list->tail;
			}
			list->tail = temp;

		}
	}
	return 1;
}

void test_destroy(struct TestList *list)
{
	struct TestNode *current;

	if(list->tail == NULL)
	{
		return;
	}
	current = list->tail->prev;
	while(current != NULL)
	{
		free(current->next->reading);
		free(current->next->writing);
		free(current->next->meaning);
		free(current->next);
		current = current->prev;
	}
	free(list->head->reading);
	free(list->head->writing);
	free(list->head->meaning);
	free(list->head);
}
