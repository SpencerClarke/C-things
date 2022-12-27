#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SHM_OUT_SIZE 2048
#define SHM_ID_SIZE 256
#define SHM_ARGC_SIZE 10
#define COMMAND_NAME_LENGTH 255
#define COMMAND_ARGUMENT_LENGTH 255
#define MAX_COMMAND_COUNT 10

int _int_to_str(char *str, int in);
void int_to_str(char *str, int in);
int _int_to_str(char *str, int in)
{
	int retval;
	if(in == 0)
		return 0;
	retval = _int_to_str(str, in / 10);
	str[retval] = '0' + (in % 10);
	return retval+1;
}
void int_to_str(char *str, int in)
{
	int retval;

	if(in == 0)
	{
		strcpy(str, "0");
		return;
	}
	if(in < 0)
	{
		retval = _int_to_str(str+1, -in);
		str[0] = '-';
		str[retval+1] = '\0';
	}
	else
	{
		retval = _int_to_str(str, in);
		str[retval] = '\0';
	}
}
