#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <wchar.h>
#define LOGOGRAPHIC_WHITESPACE 12288

int main(int argc, char **argv)
{
	FILE *fin, *fout;
	wchar_t c;
	wchar_t **columns;
	int height, width;
	int i, j;
	
	if(argc < 2)
	{
		printf("No width specified\n");
		exit(1);
	}
	else if(argc < 3)
	{
		printf("No height specified\n");
		exit(1);
	}

	width = atoi(argv[1]);
	height = atoi(argv[2]);
	if(width < 1 || height < 1)
	{
		printf("Specified pages have no characters\n");
		exit(1);
	}
	fin = argc > 3 ? fopen(argv[3], "r") : stdin;
	fout = argc == 5 ? fopen(argv[4], "w") : stdout;
	if(!fin)
	{
		printf("File '%s' not found\n", argv[3]);
		exit(1);
	}
	if(!fout)
	{
		printf("Failed to open output file\n");
		exit(1);
	}
	setlocale(LC_CTYPE,"C.UTF-8");

	c = getwc(fin);
	while(c != WEOF)
	{
		columns = malloc(sizeof(wchar_t * ) * width);
		if(columns == NULL)
			exit(5);

		for(i = 0; i < width; i++)
		{
			columns[i] = malloc(sizeof(wchar_t) * height);
			if(columns[i] == NULL)
				exit(5);

			for(j = 0; j < height; j++)
				columns[i][j] = LOGOGRAPHIC_WHITESPACE;
		}

		i = j = 0;
		while(i < width && c != WEOF)
		{
			if(j == height)
			{
				j = 0;
				i++;
				continue;
			}
			else if(c != '\n')
			{
				columns[i][j] = c;
				j++;
			}
			c = getwc(fin);
		}
		for(i = 0; i < height; i++)
		{
			for(j = width-1; j >= 0; j--)
			{
				putwc(columns[j][i], fout);
				putwc(' ', fout);
			}
			putwc('\n', fout);
		}
		putwc('\n', fout);

		for(i = 0; i < width; i++)
			free(columns[i]);
		free(columns);
	}

	fclose(fin);
	fclose(fout);

	return 0;
}