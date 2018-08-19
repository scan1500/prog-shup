#define AUTHOR "Andreas Schwarzmann"
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

void sig_handler(int signo)
{
	static int cnt = 3;
	if (signo == SIGTERM)
		printf("inc. signal: SIGTERM - ending in: %d\n", --cnt);
	if (signo == SIGINT)
		printf("inc. signal: SIGINT - ending in: %d\n", --cnt);
	if (cnt == 0)
		exit(5);
}

void main(void)
{
	printf("\nThis solution has been created by %s\n\n", AUTHOR);

	printf("\nprogram running...\n");
	printf("handling signal: SIGTERM - executed with \"kill -15 <pid>\"\n");
	printf("handling signal: SIGINT  - executed with \"kill  -2 <pid>\"\n");

	signal(SIGTERM, sig_handler);
	signal(SIGINT, sig_handler);

	while (1);
}