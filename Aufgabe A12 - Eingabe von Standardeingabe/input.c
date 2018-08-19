#define AUTHOR "Andreas Schwarzmann"
#define NAME_LEN 16
#include <stdlib.h>
#include <stdio.h>


void main(int argc, char *argv[])
{
	printf("\nThis solution has been created by %s\n\n", AUTHOR);

	int i, name_entries = *argv[1] - 48;

	char **buffer = malloc(name_entries * sizeof(char *));
	if (buffer == NULL)
		exit(1);
	else
	{
		for (i = 0; i < name_entries; i++)
		{
			buffer[i] = malloc(NAME_LEN * sizeof(char));
			if (buffer[i] == NULL)
				exit(1);
		}
	}

	for (i = 0; i < name_entries; i++)
		fgets(buffer[i], 16, stdin);

	for (i = name_entries-1; i >= 0; i--)
		printf("%s", buffer[i]);

	for (i = 0; i < name_entries; i++)
		free(buffer[i]);
	free(buffer);
}