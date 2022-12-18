#include <stdio.h>
#include <stdlib.h>

struct string
{
	char *content;
	int length;
	int size;
};

struct string new_string(char *a);
int strings_equal(struct string stra, struct string strb);
int string_comp(struct string stra, struct string strb);
void string_slice(struct string stra, int from, int to);

int main(void)
{
	struct string str, str2;
	str = new_string("abcdef");
	string_slice(str, 0, 5);
	puts(str.content);
	return 0;
}

struct string new_string(char *a)
{
	int i;
	struct string out;

	out.length = 0;
	out.content = (char *)malloc(sizeof(char)*2);
	if(out.content == NULL)
	{
		printf("Failed to allocate memory for string\n");
		exit(2);
	}
	out.size = 2;
	for(i = 0; a[i] != '\0'; i++)
	{
		if(out.length > out.size)
		{
			out.content = (char *)realloc(out.content, sizeof(char)*out.size*2);
			if(out.content == NULL)
			{
				printf("Failed to allocate memory for string\n");
				exit(2);
			}
			out.size *= 2;
		}
		out.content[i] = a[i];
		out.length++;
	}
	out.content[i] = '\0';
	return out;
}
int strings_equal(struct string stra, struct string strb)
{
	int i;

	if(stra.length != strb.length)
		return 0;

	for(i = 0; i < stra.length && stra.content[i] == strb.content[i]; i++);
	return i == stra.length;
}
int string_comp(struct string stra, struct string strb)
{
	int i;
	
	for(i = 0;;i++)
		if(stra.content[i] != strb.content[i] || stra.content[i] == '\0')
			return stra.content[i] - strb.content[i];

	printf("Invalid string\n");
	exit(2);
}

void string_slice(struct string stra, int from, int to)
{
	int i, j;
	int new_size;
	int new_length;

	new_length = to - from;
	new_size = stra.size;
	while(new_size > new_length)
	{
		new_size /= 2;
	}
	new_size *= 2;

	if(from == 0)
		stra.content[to] = '\0';
	else
	{
		j = 0;
		for(i = from; i < to; i++)
		{
			stra.content[j] = stra.content[i];
			j++;
		}
		stra.content[j] = '\0';
	}
	if(new_size != stra.size)
	{
		stra.content = (char *)realloc(stra.content, sizeof(char) * new_size);
		stra.size = new_size;
	}
	stra.length = new_length;
}
