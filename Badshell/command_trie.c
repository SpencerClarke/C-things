#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
struct Node
{
    int child_count;
    int child_size;
    int is_key;
    
    char *command; 

    char key_part;
     
    struct Node **children;
    struct Node *parent;
};
struct Trie
{
    struct Node *root;
};

struct Trie create_trie(void);
void add_key(struct Trie *trie, char *key, char *command);
char *get_command(struct Trie *trie, char *key);
int has_key(struct Trie *trie, char *key);
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

void add_key(struct Trie *trie, char *key, char *command)
{
    int i;
    int j;
    int found;
    struct Node *current_node;
    struct Node *new_node;

    current_node = trie->root;
    
    for(i = 0; key[i] != '\0'; i++)
    {
        found = 0;
        for(j = 0; j < current_node->child_count; j++)
        {
            if(current_node->children[j]->key_part == key[i])
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
            new_node->key_part = key[i];
            new_node->is_key = 0;
            current_node->children[current_node->child_count++] = new_node;
            current_node = new_node;
        }
    }
    current_node->is_key = 1;
    current_node->command = malloc(sizeof(char) * (strlen(command)+1));
    strcpy(current_node->command, command);
}

char *get_command(struct Trie *trie, char *key)
{
    int i;
    int j;
    int found;
    struct Node *current_node;

    current_node = trie->root;
    
    for(i = 0; key[i] != '\0'; i++)
    {
        found = 0;
        for(j = 0; j < current_node->child_count; j++)
        {
            if(current_node->children[j]->key_part == key[i])
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
    if(current_node->is_key)
        return current_node->command;
    else
    {
        printf("Key not found\n");
        exit(2);
    }
}
int has_key(struct Trie *trie, char *key)
{
    int i;
    int j;
    int found;
    struct Node *current_node;

    current_node = trie->root;
    
    for(i = 0; key[i] != '\0'; i++)
    {
        found = 0;
        for(j = 0; j < current_node->child_count; j++)
        {
            if(current_node->children[j]->key_part == key[i])
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
    if(current_node->is_key)
        return 1;
    else
    {
        return 0;
    }
}
void _destroy(struct Node *current)
{
    int i;

    for(i = 0; i < current->child_count; i++)
        _destroy(current->children[i]);
    free(current->children);
    free(current->command);
    free(current);
}
void destroy(struct Trie *trie)
{
    _destroy(trie->root);
}