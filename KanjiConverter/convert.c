#include <locale.h>
#include <wchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "../Datastructs/Trie/kanjitrie.c"

#define MAX_SIZE 256
int main(int argc, char **argv)
{
    struct KanjiTrie trie;
    wchar_t word[MAX_SIZE];
    wchar_t writing[MAX_SIZE];
    wchar_t input[MAX_SIZE];
    wchar_t **writings;
    int writings_count;
    wchar_t c;
    int flip;
    int i;
    int j;
    FILE *fin;

    wchar_t finished_building[] = L"Finished building.\n";
    wchar_t enter_word[] = L"Enter word: ";

    if(argc < 2 || (fin = fopen(argv[1], "r")) == NULL)
    {
        printf("Bad input\n");
        return 2;
    }

	setlocale(LC_CTYPE,"C.UTF-8");

    trie = create_trie();
    
	i = j = 0;
    flip = 0;
    writing[0] = '\0';
	while((c = getwc(fin)) != WEOF)
	{
        if(flip)
        {
            if(c == '\n')
            {
                writing[j] = '\0';
                add_word(&trie, word, writing, INT_MAX-i);
                i++;
                j = 0;
                flip = 0;
            }
            else if(j < MAX_SIZE-1)
                    writing[j++] = c;
        }
        else
        {
            if(c == '/')
            {
                word[j] = '\0';
                j = 0;
                flip = 1;
            }
            else if(j < MAX_SIZE-1)
                word[j++] = c;
        }
	}
    fclose(fin);
    for(i = 0; finished_building[i] != '\0'; i++)
        putwchar(finished_building[i]); 
    writings = malloc(sizeof(wchar_t * ) * 256);
    destroy(&trie);
    free(writings);
    return 0;
    while(1)
    {
        for(i = 0; enter_word[i] != '\0'; i++)
            putwchar(enter_word[i]); 

        i = 0;
        while((c = getwchar()) != '\n' && c != WEOF)
        {
            if(i < MAX_SIZE-1)
                input[i++] = c;
        }
        input[i] = '\0';
        if(input[0] == '\0')
            break;

        writings_count = get_writings(&trie, input, writings, 100);
        for(i = 0; i < writings_count; i++)
        {
            for(j = 0; writings[i][j] != '\0'; j++)
                putwchar(writings[i][j]);
            putwchar('\n');
        }
        putwchar('\n');
    }
    destroy(&trie);
    free(writings);
    return 0;
}
