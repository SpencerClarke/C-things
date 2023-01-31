#include <stdio.h>
#include <stdlib.h>
struct Set
{
	int bytes;
	char *members;
};

char powtwo[8] = {1, 2, 4, 8, 16, 32, 64, 128};

struct Set init(int size);

void add(struct Set *set, int num);
int exists(struct Set *set, int num);

struct Set uni(struct Set *a, struct Set *b);
struct Set inter(struct Set *a, struct Set *b);

void destroy(struct Set *set);

int main(void)
{
	struct Set a;
	struct Set b;
	struct Set u;
	
	a = init(10);
	b = init(10);
	add(&a, 3);
	add(&b, 4);

	u = uni(&a, &b);

	printf("%d\n", exists(&u, 3));
	printf("%d\n", exists(&u, 5));
	printf("%d\n", exists(&u, 4));
	
	destroy(&a);
	destroy(&b);
	destroy(&u);

	return 0;
}
struct Set init(int bytes)
{
	struct Set out;

	out.bytes = bytes;
	out.members = (char *)calloc(bytes, sizeof(char));
	return out;
}
void add(struct Set *set, int num)
{
	int frame;
	int offset;

	frame = num / (sizeof(char)*8);
	offset = num % (sizeof(char)*8);

	set->members[frame] = set->members[frame] | powtwo[offset];
}
int exists(struct Set *set, int num)
{
	int frame;
	int offset;
	char comp;

	frame = num / (sizeof(char)*8);
	offset = num % (sizeof(char)*8);
	
	comp = powtwo[offset];
	comp = set->members[frame] & comp;

	return comp;
}
struct Set uni(struct Set *a, struct Set *b)
{
	struct Set out;
	int i;

	out.members = (char *)calloc(a->bytes, sizeof(char)*8); 
	for(i = 0; i < a->bytes; i++)
		out.members[i] = a->members[i] | b->members[i];

	return out;
}
struct Set inter(struct Set *a, struct Set *b)
{
	struct Set out;
	int i;

	out.members = (char *)calloc(a->bytes, sizeof(char)*8);

	for(i = 0; i < a->bytes; i++)
		out.members[i] = a->members[i] & b->members[i];

	return out;
}
void destroy(struct Set *set)
{
	free(set->members);
}
