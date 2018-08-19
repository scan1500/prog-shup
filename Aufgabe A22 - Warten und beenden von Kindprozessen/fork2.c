#define AUTHOR "Andreas Schwarzmann"

#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>

void main(int argc, char *argv[])
{
	printf("\nThis solution has been created by %s\n\n", AUTHOR);

	int i, pid[4], stat;
	pid[0] = getpid();

	printf("\npid[0]: %d\t1.param.: %s\n\n", pid[0], argv[1]);

	for (i = 1; i <= 3; i++)
		if (getpid() == pid[0])
		{
			pid[i] = fork();	
			if (pid[i] == 0)
				pid[i] = getpid();	
		}

	if (getpid() == pid[0])
	{
		sleep(2);

		kill(pid[1], SIGTERM);
		kill(pid[2], SIGKILL);
		kill(pid[3], SIGKILL);

		for (i = 0; i < 3; i++)
			printf("(end)status: %d of pid: %d\n", stat, wait(&stat));
		exit(0);
	}

	for (i = 1; i <= 3; i++)
		if (getpid() == pid[i])
			printf("pid[%d]: %d \t %d.param.: %s\n\n", i, pid[i], i+1, argv[i+1]);

	if (getpid() == pid[1] || getpid() == pid[2])
		while (1);

	if (getpid() == pid[3])
	{
		sleep(1);
		exit(2);
	}
}