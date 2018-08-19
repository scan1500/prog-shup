#define AUTHOR "Andreas Schwarzmann"

#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

void main(int argc, char *argv[])
{
	printf("\nThis solution has been created by %s\n\n", AUTHOR);

	int stat;
	if (fork() == 0)
		if (execl("./p-new1", "./p-new1", NULL) == -1)
		{
			printf("an error occurred -> ending process\n");
			exit(1);
		}
	printf("(end)status: %d of pid: %d\n\n", stat, wait(&stat));

	//----------------------------------------------------------

	if (fork() == 0)
		if (execl("./p-new2", "./p-new2", NULL) == -1)
		{
			printf("an error occurred -> ending process\n");
			exit(1);
		}
	printf("(end)status: %d of pid: %d\n\n", stat, wait(&stat));

	//----------------------------------------------------------

	if (fork() == 0)
		if (execv("./p-new3", argv) == -1)
		{
			printf("an error occurred -> ending process\n");
			exit(1);
		}
	printf("(end)status: %d of pid: %d\n\n", stat, wait(&stat));
}