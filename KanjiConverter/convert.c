#include <locale.h>
#include <wchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "../Datastructs/Trie/kanjitrie.c"
int main(int argc, char **argv)
{
    struct Trie trie;
    wchar_t word[256];
    wchar_t writing[256];
    wchar_t input[256];
    wchar_t **writings;
    int writings_count;
    wchar_t c;
    int flip;
    int i;
    int j;
    FILE *fin;


    if(argc < 2 || (fin = fopen(argv[1], "r")) == NULL)
    {
        printf("Bad input\n");
        return 2;
    }

	setlocale(LC_CTYPE,"C.UTF-8");

    trie = create_trie();

	fin = fopen(argv[1], "r");
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
            else
            {
                writing[j++] = c;
            }
        }
        else
        {
            if(c == '/')
            {
                word[j] = '\0';
                j = 0;
                flip = 1;
            }
            else
            {
                word[j++] = c;
            }
        }
	}
    fclose(fin);
    wprintf(L"Finished building\n");
    writings = malloc(sizeof(wchar_t * ) * 256);
    
    while(1)
    {
        wprintf(L"Please enter a word in hiragana: ");
        for(i = 0; i < 255 && (c = getwchar()) != '\n' && c != WEOF; i++)
        {
            input[i] = c;
        }
        input[i] = '\0';
        if(input[0] == '\0')
            break;

        writings_count = get_writings(&trie, input, writings, 100);
        for(i = 0; i < writings_count; i++)
        {
            wprintf(L"%ls\n", writings[i]);
        }
        wprintf(L"\n");
    }
    destroy(&trie);
    return 0;
}
