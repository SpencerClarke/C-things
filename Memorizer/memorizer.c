#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <time.h>
#include <locale.h>

#define IN_MEANING 0
#define IN_KANA 1
#define IN_KANJI 2


struct Node
{
	int value;
	struct Node *next;
};
FILE *safe_open(char *filename, char *mode);
void *safe_malloc(int size);

void read_answer(wchar_t *in, int max_size);
void wide_print(wchar_t *in);
void wide_print_integer(int in);

int main(int argc, char **argv)
{
	FILE *fin;

	int remaining_terms;
	int term_count;
	int max_size;
	int *kanji_sizes;
	int *kana_sizes;
	int *meanings_sizes;
	wint_t c;
	wint_t prev;
	
	wchar_t **kanji;
	wchar_t **kana;
	wchar_t **meanings;
	int *kana_only;

	wchar_t *answer;
	
	time_t t;

	int state;

	int i, j;
	int temp;


	int *term_order;

	if(argc < 2)
	{
		printf("No filename given\n");
		exit(1);
	}

	setlocale(LC_CTYPE,"C.UTF-8");

	/*First pass, count number of terms in set*/
	fin = safe_open(argv[1], "r");

	term_count = 0;
	while((c = getwc(fin)) != WEOF)
	{
		if(c == '\n')
			term_count++;
		prev = c;
	}
	if(prev != '\n')
	{
		printf("No newline at end of file\n");
		exit(2);
	}

	fclose(fin);

	/*Allocate enough memory to store pointers to all the terms*/
	kanji = safe_malloc(sizeof(wchar_t * ) * term_count);
	kana = safe_malloc(sizeof(wchar_t * ) * term_count);
	meanings = safe_malloc(sizeof(wchar_t * ) * term_count);

	kanji_sizes = safe_malloc(sizeof(int) * term_count);
	kana_sizes = safe_malloc(sizeof(int) * term_count);
	meanings_sizes = safe_malloc(sizeof(int) * term_count);

	kana_only = safe_malloc(sizeof(int) * term_count);

	/*Start each size at 1 to account for null terminator*/
	/*Fill kana_only array with all zeros*/
	for(i = 0; i < term_count; i++)
	{
		kanji_sizes[i] = kana_sizes[i] = meanings_sizes[i] = 1;
		kana_only[i] = 0;
	}
	/*Second pass, count the size of each term*/
	/*Determine which terms are kana-only*/
	fin = safe_open(argv[1], "r");
	
	i = 0;
	max_size = 0;
	state = IN_MEANING;
	while((c = getwc(fin)) != WEOF)
	{
		if(state == IN_MEANING)
		{
			if(c == ',')
			{
				if(meanings_sizes[i] > max_size)
					max_size = meanings_sizes[i];
				state = IN_KANA;
			}
			else if(c == '\n')
			{
				printf("File is improperly formatted on line %d\n", i+1);
				exit(1);
			}
			else
				meanings_sizes[i]++;
		}
		else if(state == IN_KANA)
		{
			if(c == ',')
			{
				if(kana_sizes[i] > max_size)
					max_size = kana_sizes[i];
				state = IN_KANJI;
			}
			else if(c == '\n')
			{
				if(kana_sizes[i] > max_size)
					max_size = kana_sizes[i];
				kana_only[i] = 1;
				state = IN_MEANING;
			}
			else
				kana_sizes[i]++;
		}
		else if(state == IN_KANJI)
		{
			if(c == '\n')
			{
				if(kanji_sizes[i] > max_size)
					max_size = kanji_sizes[i];
				i++;
				state = IN_MEANING;
			}
			else
				kanji_sizes[i]++;
		}
	}
	fclose(fin);

	for(i = 0; i < term_count; i++)
	{
		meanings[i] = safe_malloc(sizeof(wchar_t) * meanings_sizes[i]);
		kana[i] = safe_malloc(sizeof(wchar_t) * kana_sizes[i]);
		if(!kana_only[i])
			kanji[i] = safe_malloc(sizeof(wchar_t) * kanji_sizes[i]);
	}
	answer = safe_malloc(sizeof(wchar_t) * (max_size+1));

	free(meanings_sizes);
	free(kana_sizes);
	free(kanji_sizes);

	/*Third pass, read terms into allocated memory blocks*/
	fin = safe_open(argv[1], "r");

	state = IN_MEANING;
	i = j = 0;
	while((c = getwc(fin)) != WEOF)
	{		
		switch(state)
		{
			case IN_MEANING:
				if(c == ',')
				{
					meanings[i][j] = '\0';
					j = 0;
					state = IN_KANA;
				}
				else
					meanings[i][j++] = c;
				break;
			case IN_KANA:
				if(c == ',')
				{
					kana[i][j] = '\0';
					j = 0;
					state = IN_KANJI;
				}
				else if(c == '\n')
				{
					kana[i][j] = '\0';
					j = 0;
					state = IN_MEANING;
					continue;
				}
				else
					kana[i][j++] = c;
				break;
			case IN_KANJI:
				if(c == '\n')
				{
					kanji[i][j] = '\0';
					j = 0;
					i++;
					state = IN_MEANING;
				}
				else
					kanji[i][j++] = c;
				break;
		}
	}
	fclose(fin);

	srand((unsigned) time(&t));

	/*term_order = {0,1,2,3,... term_count-1}*/
	term_order = safe_malloc(sizeof(int) * term_count);
	for(i = 0; i < term_count; i++)
		term_order[i] = i;
	
	/*perform Fisher-Yates shuffle on term order*/
	for(i = term_count-1; i > 0; i--)
	{
		j = rand() % (i+1);
		temp = term_order[i];
		term_order[i] = term_order[j];
		term_order[j] = temp;
	}
	remaining_terms = term_count;
	i = 0;
	while(i < term_count)
	{
		wide_print(L"Remaining terms: ");
		wide_print_integer(remaining_terms);
		putwchar('\n');

		wide_print(meanings[term_order[i]]);
		wide_print(L": ");
		read_answer(answer, max_size);
		if(kana_only[term_order[i]] == 1)
		{
			if(wcscmp(answer, kana[term_order[i]]) == 0)
			{
				wide_print(L"Correct. \n");
				i++;
				remaining_terms--;
			}
			else
			{

				wide_print(L"Incorrect.\nAnswer: ");
				wide_print(kana[term_order[i]]);
				putwchar('\n');
				temp = term_order[i];
				for(j = i+1; j < term_count; j++)
				{
					term_order[j-1] = term_order[j];
				}
				term_order[term_count-1] = temp;
			}
			putwchar('\n');
		}
		else
		{
			if(wcscmp(answer, kanji[term_order[i]]) == 0)
			{
				wide_print(L"Correct.\n");
				i++;
				remaining_terms--;
			}
			else
			{
				wide_print(L"Incorrect.\nAnswer: ");
				wide_print(kanji[term_order[i]]);
				putwchar('\n');
				wide_print(L"Pronunciation: ");
				wide_print(kana[term_order[i]]);
				putwchar('\n');
				temp = term_order[i];
				for(j = i+1; j < term_count; j++)
				{
					term_order[j-1] = term_order[j];
				}
				term_order[term_count-1] = temp;
			}
			putwchar('\n');
		}
	}
	wide_print(L"Good job\n");
	return 0;
}
FILE *safe_open(char *filename, char *mode)
{
	FILE *fin;

	fin = fopen(filename, mode);
	if(!fin)
	{
		printf("Failed to open file '%s'\n", filename);
		exit(1);
	}
	return fin;
}
void *safe_malloc(int size)
{
	void *out = malloc(size);
	if(out == NULL)
	{
		printf("Failed to allocate memory\n");
		exit(1);
	}
	return out;
}
void read_answer(wchar_t *in, int max_size)
{
	wchar_t c;

	while((c = getwchar()) && c != '\n')
		if(max_size-- > 0)
			*(in++) = c;

	*in = '\0';
}
void wide_print(wchar_t *in)
{
	wchar_t c;
	while((c = *(in++)) != '\0')
		putwchar(c);
}
void wide_print_integer(int in)
{
	if(in < 10)
	{
		putwchar('0' + in);
		return;
	}
	wide_print_integer(in/10);
	putwchar('0' + (in % 10));
}
