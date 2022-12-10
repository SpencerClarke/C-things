#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Node
{
    int child_count;
    int child_size;
    int valid;

    char value;
     
    struct Node **children;
};
struct Trie
{
    struct Node *root;
};

struct Trie create_trie(void);
void add_word(struct Trie *trie, char *word);
int has_word(struct Trie *trie, char *word);

int main(int argc, char **argv)
{
    struct Trie trie;
    char word[256];
    
    FILE *fin;

    if(argc < 3 || (fin = fopen(argv[1], "r")) == NULL)
    {
        printf("Bad input\n");
        return 2;
    }

    trie = create_trie();
    while(fgets(word, 256, fin) != NULL)
    {
        word[strlen(word)-1] = '\0';
        add_word(&trie, word);
    }
    if(has_word(&trie, argv[2]))
        printf("\"%s\" was found.\n", argv[2]);
    else
        printf("\"%s\" was not found.\n", argv[2]);
    return 0;
}

struct Trie create_trie(void)
{
    struct Trie out;

    out.root = malloc(sizeof(struct Node));
    out.root->children = malloc(sizeof(struct Node * ) * 8);
    out.root->child_size = 8;
    out.root->child_count = 0;

    return out;
}

void add_word(struct Trie *trie, char *word)
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
            new_node->children = malloc(sizeof(struct Node) * 8);
            new_node->child_count = 0;
            new_node->child_size = 8;
            new_node->value = word[i];
            new_node->valid = 0;
            current_node->children[current_node->child_count++] = new_node;
            current_node = new_node;
        }
    }
    current_node->valid = 1;
}

int has_word(struct Trie *trie, char *word)
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
            return 0;
        }
    }
    return current_node->valid;
}