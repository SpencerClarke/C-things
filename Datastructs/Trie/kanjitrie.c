#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <locale.h>
#include <wchar.h>

struct Node
{
    int child_count;
    int child_size;
    int is_word;

    int writing_count;
    int *commonality_order;
    wchar_t **writing_list;

    wchar_t key_part;

    struct Node **children;
    struct Node *parent;
};
struct Trie
{
    struct Node *root;
};

struct Trie create_trie(void);
void add_word(struct Trie *trie, wchar_t *word, wchar_t *writing, int writing_commonality);
int get_writings(struct Trie *trie, wchar_t *word, wchar_t **buffer, int buff_size);
void delete(struct Trie *trie, wchar_t *word);
int has_key(struct Trie *trie, wchar_t *word);
void _destroy(struct Node *current);
void destroy(struct Trie *trie);

struct Trie create_trie(void)
{
    struct Trie out;

    out.root = malloc(sizeof(struct Node));
    out.root->children = malloc(sizeof(struct Node *));
    out.root->child_size = 1;
    out.root->child_count = 0;
    out.root->writing_count = 0;
    out.root->writing_list = NULL;
    out.root->parent = NULL;

    return out;
}

void add_word(struct Trie *trie, wchar_t *word, wchar_t *writing, int writing_commonality)
{
    int i;
    int j;
    int length;
    int found;
    int insertion_index;

    struct Node *current_node;
    struct Node *new_node;

    current_node = trie->root;

    for (i = 0; word[i] != '\0'; i++)
    {
        found = 0;
        for (j = 0; j < current_node->child_count; j++)
        {
            if (current_node->children[j]->key_part == word[i])
            {
                current_node = current_node->children[j];
                found = 1;
                break;
            }
        }
        if (!found)
        {
            if (current_node->child_count >= current_node->child_size)
            {
                current_node->children = realloc(current_node->children, sizeof(struct Node * ) * current_node->child_size * 2);
                current_node->child_size *= 2;
            }
            new_node = malloc(sizeof(struct Node));
            new_node->children = malloc(sizeof(struct Node) * 1);
            new_node->child_count = 0;
            new_node->writing_count = 0;
            new_node->child_size = 1;
            new_node->parent = current_node;
            new_node->key_part = word[i];
            new_node->is_word = 0;
            current_node->children[current_node->child_count++] = new_node;
            current_node = new_node;
        }
    }
    length = wcslen(writing);
    if(current_node->is_word)
    {
        current_node->writing_list = realloc(current_node->writing_list, sizeof(wchar_t * ) * (current_node->writing_count+1));
        current_node->commonality_order = realloc(current_node->commonality_order, sizeof(int) * (current_node->writing_count+1));
        for(i = 0; i < current_node->writing_count; i++)
            if(current_node->commonality_order[i] < writing_commonality)
                break;
        
        insertion_index = i;
        for(i = current_node->writing_count; i > insertion_index; i--)
        {
            current_node->writing_list[i] = current_node->writing_list[i-1];
            current_node->commonality_order[i] = current_node->commonality_order[i-1];
        }
        current_node->commonality_order[insertion_index] = writing_commonality;
        current_node->writing_list[insertion_index] = (wchar_t *)malloc(sizeof(wchar_t) * (length + 1));
        for(j = 0; j < length; j++)
            current_node->writing_list[insertion_index][j] = writing[j];
        current_node->writing_list[insertion_index][j] = '\0';
        current_node->writing_count++;
    }
    else
    {
        current_node->is_word = 1;
        current_node->writing_list = malloc(sizeof(wchar_t * ));
        current_node->writing_list[0] = malloc(sizeof(wchar_t) * (length + 1));
        current_node->commonality_order = malloc(sizeof(int));
        for(j = 0; j < length; j++)
            current_node->writing_list[0][j] = writing[j];
        current_node->writing_list[0][j] = '\0';
        current_node->writing_count = 1;
        current_node->commonality_order[0] = writing_commonality;
    }
}

int has_key(struct Trie *trie, wchar_t *word)
{
    int i;
    int j;
    int found;
    struct Node *current_node;

    current_node = trie->root;

    for (i = 0; word[i] != '\0'; i++)
    {
        found = 0;
        for (j = 0; j < current_node->child_count; j++)
        {
            if (current_node->children[j]->key_part == word[i])
            {
                current_node = current_node->children[j];
                found = 1;
                break;
            }
        }
        if (!found)
        {
            return 0;
        }
    }
    if (current_node->is_word)
        return 1;
    else
    {
        return 0;
    }
}
int get_writings(struct Trie *trie, wchar_t *word, wchar_t **buffer, int buff_size)
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
            return 0;
    }
    if(current_node->is_word)
    {
        for(i = 0; i < buff_size && i < current_node->writing_count; i++)
            buffer[i] = current_node->writing_list[i];
        return i;
    }
    return 0;
}
void delete(struct Trie *trie, wchar_t *word)
{
    int i;
    int j;
    int found;
    int length;
    wchar_t *new_word;
    struct Node *current_node;
    struct Node *previous_node;

    if (word[0] == '\0')
    {
        trie->root->is_word = 0;
        return;
    }
    current_node = trie->root;

    for (i = 0; word[i] != '\0'; i++)
    {
        found = 0;
        for (j = 0; j < current_node->child_count; j++)
        {
            if (current_node->children[j]->key_part == word[i])
            {
                previous_node = current_node;
                current_node = current_node->children[j];
                found = 1;
                break;
            }
        }
        if (!found)
        {
            return;
        }
    }
    if (current_node->child_count == 0)
    {
        previous_node->children[j] = NULL;
        previous_node->child_count--;
        if (j == 0)
            previous_node->children[0] = previous_node->children[previous_node->child_count];

        free(current_node->children);
        free(current_node);
        if (!previous_node->is_word)
        {
            length = wcslen(word);
            new_word = malloc(sizeof(wchar_t) * length);
            for (i = 0; i < length - 1; i++)
            {
                new_word[i] = word[i];
            }
            new_word[i] = '\0';
            delete (trie, new_word);
            free(new_word);
        }
    }
    else
    {
        current_node->is_word = 0;
        return;
    }
}

void _destroy(struct Node *current)
{
    int i;
    for(i = 0; i < current->child_count; i++)
        _destroy(current->children[i]);
    
    if(current->is_word)
    {
        for(i = 0; i < current->writing_count; i++)
        {
            free(current->writing_list[i]);
        }
        free(current->writing_list);
    }
    free(current->children);
    free(current);
}
void destroy(struct Trie *trie)
{
    _destroy(trie->root);
}