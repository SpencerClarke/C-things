#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
struct Node
{
    int child_count;
    int child_size;
    int is_word;
    int value; 

    char key_part;
     
    struct Node **children;
    struct Node *parent;
};
struct Trie
{
    struct Node *root;
};

struct Trie create_trie(void);
void add_word(struct Trie *trie, char *word, int value);
int index_of(struct Trie *trie, char *word);
struct Node *_get_highest_node(struct Node *current, int current_max);
int get_highest_key(struct Trie *trie, char *buffer, char *start, int buff_size);
int has_key(struct Trie *trie, char *word);
void _destroy(struct Node *current);
void destroy(struct Trie *trie);

struct Trie create_trie(void)
{
    struct Trie out;

    out.root = malloc(sizeof(struct Node));
    out.root->children = malloc(sizeof(struct Node * ));
    out.root->child_size = 1;
    out.root->child_count = 0;
    out.root->parent = NULL;

    return out;
}

void add_word(struct Trie *trie, char *word, int value)
{
    int i;
    int j;
    int found;
    struct Node *current_node;
    struct Node *new_node;

    current_node = trie->root;
    
    for(i = 0; word[i] != '\0'; i++)
    {
        found = 0;
        for(j = 0; j < current_node->child_count; j++)
        {
            if(current_node->children[j]->key_part == word[i])
            {
                current_node = current_node->children[j];
                found = 1;
                break;
            }
        }
        if(!found)
        {
            if(current_node->child_count >= current_node->child_size)
            {
                current_node->children = realloc(current_node->children, sizeof(struct Node * ) * current_node->child_size*2);
                current_node->child_size *= 2;
            }
            new_node = malloc(sizeof(struct Node));
            new_node->children = malloc(sizeof(struct Node) * 1);
            new_node->child_count = 0;
            new_node->child_size = 1;
            new_node->parent = current_node;
            new_node->key_part = word[i];
            new_node->is_word = 0;
            current_node->children[current_node->child_count++] = new_node;
            current_node = new_node;
        }
    }
    current_node->is_word = 1;
    current_node->value = value;
}

int index_of(struct Trie *trie, char *word)
{
    int i;
    int j;
    int found;
    struct Node *current_node;

    current_node = trie->root;
    
    for(i = 0; word[i] != '\0'; i++)
    {
        found = 0;
        for(j = 0; j < current_node->child_count; j++)
        {
            if(current_node->children[j]->key_part == word[i])
            {
                current_node = current_node->children[j];
                found = 1;
                break;
            }
        }
        if(!found)
        {
            printf("Key not found\n");
            exit(2);
        }
    }
    if(current_node->is_word)
        return current_node->value;
    else
    {
        printf("Key not found\n");
        exit(2);
    }
}

int has_key(struct Trie *trie, char *word)
{
    int i;
    int j;
    int found;
    struct Node *current_node;

    current_node = trie->root;
    
    for(i = 0; word[i] != '\0'; i++)
    {
        found = 0;
        for(j = 0; j < current_node->child_count; j++)
        {
            if(current_node->children[j]->key_part == word[i])
            {
                current_node = current_node->children[j];
                found = 1;
                break;
            }
        }
        if(!found)
        {
            return 0;
        }
    }
    if(current_node->is_word)
        return 1;
    else
    {
        return 0;
    }
}
struct Node *_get_highest_node(struct Node *current, int current_max)
{
    int i;
    struct Node *true_max;
    struct Node *test_max;

    if(current->is_word)
    {
        if(current->value > current_max)
        {
            true_max = current;
            current_max = current->value;
        }
    }
    for(i = 0; i < current->child_count; i++)
    {
        test_max = _get_highest_node(current->children[i], INT_MIN);
        if(test_max->value >= current_max)
        {
            current_max = test_max->value;
            true_max = test_max;
        }
    }
    return true_max;
}
int get_highest_key(struct Trie *trie, char *buffer, char *start, int buff_size)
{

    struct Node *max;
    struct Node *root;
    int max_num;
    int found;
    int i;
    int j;

    if(buff_size < 1)
    {
        printf("Insufficient buffer size\n");
        exit(2);
    }
    root = trie->root;
    for(i = 0; start[i] != '\0'; i++)
    {
        found = 0;
        for(j = 0; j < root->child_count; j++)
        {
            if(root->children[j]->key_part == start[i])
            {
                found = 1;
                root = root->children[j];
                break;
            }
        }
        if(!found)
        {
            return 0;
        }
    }
    max = _get_highest_node(root, INT_MIN);
    max_num = max->value;
    buffer[0] = '\0';
    for(i = 0; max->parent != NULL && i < buff_size-1; i++)
    {
        for(j = i; j >= 0; j--)
            buffer[j+1] = buffer[j];
        buffer[0] = max->key_part;
        max = max->parent;
    }
    return 1;
}

void _destroy(struct Node *current)
{
    int i;

    for(i = 0; i < current->child_count; i++)
        _destroy(current->children[i]);
    free(current->children);
    free(current);
}
void destroy(struct Trie *trie)
{
    _destroy(trie->root);
}