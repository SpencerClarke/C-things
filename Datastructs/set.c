#include <stdio.h>
#include <stdlib.h>
struct Set
{
	char *members;
};

char powtwo[8] = {1, 2, 4, 8, 16, 32, 64, 128};

struct Set init(int size);
void add(struct Set *set, int num);
int exists(struct Set *set, int num);

int main(void)
{
	
	struct Set set;

	set = init(10);

	add(&set, 3);
	add(&set, 4);

	printf("%d\n", exists(&set, 3));
	printf("%d\n", exists(&set, 5));
	printf("%d\n", exists(&set, 4));

	return 0;
}
struct Set init(int bytes)
{
	struct Set out;

	out.members = (char *)calloc(bytes, sizeof(char));
	return out;
}
void add(struct Set *set, int num)
{
	int frame;
	int offset;

	frame = num / 8;
	offset = num % 8;

	set->members[frame] += powtwo[offset];
}
int exists(struct Set *set, int num)
{
	int frame;
	int offset;
	char comp;

	frame = num / 8;
	offset = num % 8;
	
	comp = powtwo[offset];
	comp = set->members[frame] & comp;

	return comp;
}
