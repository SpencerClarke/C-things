
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
struct Node
{
    int child_count;
    int child_size;
    int valid;
    int number; 

    char value;
     
    struct Node **children;
    struct Node *parent;
};
struct Trie
{
    struct Node *root;
};

struct Trie create_trie(void);
void add_word(struct Trie *trie, char *word, int number);
int index_of(struct Trie *trie, char *word);
void delete(struct Trie *trie, char *word);
struct Node *_get_highest_node(struct Node *current, int current_max);
int get_highest_value(struct Trie *trie, char *buffer, int buff_size);
void _destroy(struct Node *current);
void destroy(struct Trie *trie);

int main(int argc, char **argv)
{
    struct Trie trie;
    char word[256];
    char *highest;
    int i;
    FILE *fin;

    if(argc < 3 || (fin = fopen(argv[1], "r")) == NULL)
    {
        printf("Bad input\n");
        return 2;
    }

    trie = create_trie();

    for(i = 0; fgets(word, 256, fin) != NULL; i++)
    {
        word[strlen(word)-1] = '\0';
        add_word(&trie, word, i);
    }
    printf("\"%s\": %d\n", argv[2], index_of(&trie, argv[2]));

    highest = malloc(sizeof(char) * 256);
    printf("Highest value: %d\n", get_highest_value(&trie, highest, 256));
    printf("Highest word: %s\n", highest);

    destroy(&trie);
    fclose(fin);
    return 0;
}

struct Trie create_trie(void)
{
    struct Trie out;

    out.root = malloc(sizeof(struct Node));
    out.root->children = malloc(sizeof(struct Node * ) * 8);
    out.root->child_size = 1;
    out.root->child_count = 0;
    out.root->parent = NULL;

    return out;
}

void add_word(struct Trie *trie, char *word, int number)
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
            if(current_node->children[j]->value == word[i])
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
            new_node->value = word[i];
            new_node->valid = 0;
            current_node->children[current_node->child_count++] = new_node;
            current_node = new_node;
        }
    }
    current_node->valid = 1;
    current_node->number = number;
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
            if(current_node->children[j]->value == word[i])
            {
                current_node = current_node->children[j];
                found = 1;
                break;
            }
        }
        if(!found)
        {
            printf("Value not found\n");
            exit(2);
        }
    }
    if(current_node->valid)
        return current_node->number;
    else
    {
        printf("Value not found\n");
        exit(2);
    }
}

void delete(struct Trie *trie, char *word)
{
    int i;
    int j;
    int found;
    int length;
    char *new_word;
    struct Node *current_node;
    struct Node *previous_node;

    if(word[0] == '\0')
    {
        trie->root->valid = 0;
        return;
    }
    current_node = trie->root;
    
    for(i = 0; word[i] != '\0'; i++)
    {
        found = 0;
        for(j = 0; j < current_node->child_count; j++)
        {
            if(current_node->children[j]->value == word[i])
            {
                previous_node = current_node;
                current_node = current_node->children[j];
                found = 1;
                break;
            }
        }
        if(!found)
        {
            return;
        }
    }
    if(current_node->child_count == 0)
    {
        previous_node->children[j] = NULL;
        previous_node->child_count--;
        if(j == 0)
            previous_node->children[0] = previous_node->children[previous_node->child_count];

        free(current_node->children);
        free(current_node);
        if(!previous_node->valid)
        {
            length = strlen(word);
            new_word = malloc(sizeof(char) * length);
            for(i = 0; i < length-1; i++)
            {
                new_word[i] = word[i];
            }
            new_word[i] = '\0';
            delete(trie, new_word);
            free(new_word);
        }
    }
    else
    {
        current_node->valid = 0;
        return;
    }
}
struct Node *_get_highest_node(struct Node *current, int current_max)
{
    int i;
    struct Node *true_max;
    struct Node *test_max;

    if(current->valid)
    {
        if(current->number > current_max)
        {
            true_max = current;
            current_max = current->number;
        }
    }
    for(i = 0; i < current->child_count; i++)
    {
        test_max = _get_highest_node(current->children[i], INT_MIN);
        if(test_max->number >= current_max)
        {
            current_max = test_max->number;
            true_max = test_max;
        }
    }
    return true_max;
}
int get_highest_value(struct Trie *trie, char *buffer, int buff_size)
{
    struct Node *max;
    int max_num;
    int i, j;

    if(buff_size < 1)
    {
        printf("Insufficient buffer size\n");
        exit(2);
    }
    max = _get_highest_node(trie->root, INT_MIN);
    max_num = max->number;
    buffer[0] = '\0';
    for(i = 0; max->parent != NULL && i < buff_size-1; i++)
    {
        for(j = i; j >= 0; j--)
            buffer[j+1] = buffer[j];
        buffer[0] = max->value;
        max = max->parent;
    }
    return max_num;
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