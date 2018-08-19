#define AUTHOR "Andreas Schwarzmann"
#include <stdio.h>

void main(int argc, char *argv[])
{
	printf("\nThis solution has been created by %s\n\n", AUTHOR);

	int i;
	for (i = 0; i < argc; i++)
		printf("Line - %d: %s\n", i, argv[i]);
}