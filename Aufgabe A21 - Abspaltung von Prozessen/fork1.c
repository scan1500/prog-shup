#define AUTHOR "Andreas Schwarzmann"

#include <unistd.h>
#include <stdio.h>

void main(void)
{
	printf("\nThis solution has been created by %s\n\n", AUTHOR);

	if (fork() > 0)
		printf("ppid of parent: %d\npid  of parent: %d\n\n", getppid(), getpid());
	else
		printf("ppid of child: %d\npid  of child: %d\n\n", getppid(), getpid());

	while(1);
}