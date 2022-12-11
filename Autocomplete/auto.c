#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "../Datastructs/Trie/trie.c"
int main(int argc, char **argv)
{
    struct Trie trie;
    char word[256];
    char input[256];
    char *highest;
    int i;
    FILE *fin;

    if(argc < 2 || (fin = fopen(argv[1], "r")) == NULL)
    {
        printf("Bad input\n");
        return 2;
    }

    trie = create_trie();

    for(i = 0; fgets(word, 256, fin) != NULL; i++)
    {
        word[strlen(word)-1] = '\0';
        add_word(&trie, word, INT_MAX-i);
    }

    fclose(fin);

    highest = malloc(sizeof(char) * 256);
    while(1)
    {
        fgets(input, 256, stdin);
        input[strlen(input)-1] = '\0';

        if(strcmp(input, "stop") == 0)
        {
            printf("Goodbye\n");
            break;
        }
        if(!has_key(&trie, input))
        {
            if(get_highest_key(&trie, highest, input, 256))
                printf("Prediction: %s\n", highest);
            else
                printf("No prediction.\n");
        }
        else
            printf("Word found.\n");
    }
    destroy(&trie);
    free(highest);
    return 0;
}
