#include <stdlib.h>
#include <stdio.h>

void main(int argc, char *argv[])
{
	int i;
	for(i = 1; i < argc; i++)
		printf("argv[%d]: %s\n", i, argv[i]);
	exit(0);
}