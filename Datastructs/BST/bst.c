#include <stdio.h>
#include <stdlib.h>

struct Node
{
	int value;
	struct Node *left;
	struct Node *right;
};
struct BST
{
	struct Node *top;
};
struct BST BST_create();

void BST_insert(struct BST *bst, int value);
void _BST_insert(struct Node *node, int value);

int BST_contains(struct BST *bst, int value);
int _BST_contains(struct Node *node, int value);

void BST_remove(struct BST *bst, int value);
void _BST_remove(struct Node *node, int value);

struct Node *BST_minimum(struct BST *bst);
struct Node *_BST_minimum(struct Node *node);

int main(int argc, char **argv)
{
	int i;
       	int c;
	int ac;
	int number_to_search_for;
	int linenum;

	int *nums;

	FILE *filein;

	struct BST bst;
	
	if(argc < 2)
	{
		printf("No file given\n");
		exit(1);
	}
	if(argc < 3)
	{
		printf("No integer to search for given\n");
		exit(2);
	}

	filein = fopen(argv[1], "r");
	if(filein == NULL)
	{
		printf("File not found\n");
		exit(3);
	}

	linenum = 0;
	while((c = getc(filein)) != EOF)
	{
		if(c == '\n')
			linenum++;	
		else if(c < '0' || c > '9')
		{
			printf("Invalid number on line %d\n", linenum+1);
			exit(4);
		}
	}

	fclose(filein);
	
	nums = malloc(sizeof(int) * linenum);

	filein = fopen(argv[1], "r");
	
	ac = linenum = 0;
	while((c = getc(filein)) != EOF)
	{
		if(c == '\n')
		{
			nums[linenum++] = ac;
			ac = 0;
		}
		else
			ac = ac * 10 + c - '0';
	}
	
	fclose(filein);

	number_to_search_for = 0;
	while((c = *argv[2]) != '\0' && c >= '0' && c <= '9')
	{
		number_to_search_for = number_to_search_for * 10 + c - '0';
		argv[2]++;
	}
	if(c != '\0')
	{
		printf("Invalid number to search for given\n");
		exit(5);
	}

	bst = BST_create();

	for(i = 0; i < linenum; i++)
		BST_insert(&bst, nums[i]);

	if(BST_contains(&bst, number_to_search_for))
		printf("Number found\n");

	else
		printf("Number not found\n");

	return 0;
}

struct BST BST_create()
{
	struct BST bst;
	bst.top = NULL;
	return bst;
}
void BST_insert(struct BST *bst, int value)
{
	if(bst->top == NULL)
	{
		bst->top = malloc(sizeof(struct Node));
		bst->top->value = value;
		bst->top->left = bst->top->right = NULL;
	}
	else
	{
		_BST_insert(bst->top, value);
	}
}

void _BST_insert(struct Node *node, int value)
{
	if(value < node->value)
	{
		if(node->left == NULL)
		{
			node->left = malloc(sizeof(struct Node));
			node->left->left = node->left->right = NULL;
			node->left->value = value;
		}
		else
			_BST_insert(node->left, value);
	}
	else if(value > node->value)
	{
		if(node->right == NULL)
		{
			node->right = malloc(sizeof(struct Node));
			node->right->left = node->right->right = NULL;
			node->right->value = value;
		}
		else
			_BST_insert(node->right, value);
	}
}
int BST_contains(struct BST *bst, int value)
{
	return bst->top == NULL ? 0 : _BST_contains(bst->top, value);
}
int _BST_contains(struct Node *node, int value)
{

	if(node->value == value)
	{
		printf("%d\n", node->value);
		return 1;
	}
	else
	{
		printf("%d -> ", node->value);
		if(value > node->value)
		{
			if(node->right == NULL)
			{
				printf("end\n");
				return 0;
			}
			else
				return _BST_contains(node->right, value);
		}
		else
		{
			if(node->left == NULL)
			{
				printf("end\n");
				return 0;
			}
			else
				return _BST_contains(node->left, value);
		}
	}
	return 2;
}

struct Node *BST_minimum(struct BST *bst)
{
	if(bst->top != NULL)
		return _BST_minimum(bst->top);
	return NULL;
}
struct Node *_BST_minimum(struct Node *node)
{
	if(node->left == NULL)
	{
		if(node->right == NULL)
			return node;
		return _BST_minimum(node->right);
	}
	return _BST_minimum(node->left);
}
void BST_remove(struct BST *bst, int value)
{
	struct Node *temp, *min;
	if(bst->top != NULL)
	{
		if(bst->top->value == value)
		{
			if(bst->top->right == NULL && bst->top->left == NULL)
			{
				free(bst->top);
				bst->top = NULL;
			}
			else if(bst->top->left == NULL)
			{
				temp = bst->top;
				bst->top = bst->top->right;
				free(temp);
			}
			else if(bst->top->right == NULL)
			{
				temp = bst->top;
				bst->top = bst->top->left;
				free(temp);
			}
			else
			{
				min = _BST_minimum(bst->top->right);
				min->left = bst->top->left;
				temp = bst->top;
				bst->top = bst->top->right;
				free(temp);
			}
		}
		else
			_BST_remove(bst->top, value);
	}
}
void _BST_remove(struct Node *node, int value)
{
	struct Node *temp;
	struct Node *min;
	if(value > node->value)
	{
		if(node->right == NULL)
			return;
		else if(node->right->value == value)
		{
			if(node->right->left == NULL && node->right->right == NULL)
			{
				free(node->right);
				node->right = NULL;
			}
			else if(node->right->left == NULL)
			{
				temp = node->right;
				node->right = node->right->right;
				free(temp);
			}
			else if(node->right->right == NULL)
			{
				temp = node->right;
				node->right = node->right->left;
				free(temp);
			}
			else
			{
				min = _BST_minimum(node->right->right);
				_BST_remove(node->right->right, min->value);
				node->right->value = min->value;
			}
		}
		else
			_BST_remove(node->right, value);
	}
	else if(value < node->value)
	{
		if(node->left == NULL)
			return;
		else if(node->left->value == value)
		{
			if(node->left->left == NULL && node->left->right == NULL)
			{
				free(node->left);
				node->left = NULL;
			}
			else if(node->left->left == NULL)
			{
				temp = node->left;
				node->left = node->left->right;
				free(temp);
			}
			else if(node->left->right == NULL)
			{
				temp = node->left;
				node->left = node->left->left;
				free(temp);
			}
			else
			{
				min = _BST_minimum(node->left->right);
				_BST_remove(node->left->right, min->value);
				node->left->value = min->value;
			}
		}
		else
			_BST_remove(node->left, value);
	}
}

