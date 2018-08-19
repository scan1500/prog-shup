#define AUTHOR "Andreas Schwarzmann"

#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define DIRLEN 256
#define CMDLEN 256

void main(int argc, char *argv[])
{
	printf("\033[2JThis solution has been created by %s\n\n", AUTHOR);

	int args_cnt, i, cpid, stat;
	char cwd[DIRLEN], cmd[CMDLEN], cmd_env[CMDLEN], *cmd_ptr, **args, *pths;

	while (1)
	{
		printf("%s > ", getcwd(cwd, sizeof(cwd)));
		fgets(cmd, CMDLEN, stdin); // GET USER INPUT

		if (strncmp(cmd, "exit", 4) == 0) // ABORT IF USER INPUT = "exit"
			exit(0);

		// REPLACE SPACE AND NEWLINE WITH '\0', COUNT ARGUMENTS - (+1 EVERY REPLACEMENT, +1 NULL)
		for (args_cnt = 1, cmd_ptr = cmd; *cmd_ptr != '\0'; cmd_ptr++)
			if (*cmd_ptr == ' ' || *cmd_ptr == '\n')
			{
				*cmd_ptr = '\0';
				args_cnt++;
			}

		// ALLOCATE MEMORY: 1 CHAR POINTER FOR EVERY ARGUMENT
		if ((args = (char**)malloc((args_cnt) * sizeof(char *))) == NULL)
			exit(1);
		else // ASSIGN ARGUMENTS TO CHAR POINTERS - IF '\0' FOUND -> POINTER ON NEXT CHAR
			for (i = 1, args[0] = cmd, cmd_ptr = cmd; i < args_cnt - 1; cmd_ptr++)
				if (*cmd_ptr == '\0')
					args[i++] = cmd_ptr + 1;
		args[args_cnt - 1] = NULL; // LAST POINTER NULL

		if ((cpid = fork()) == 0)
			if (execv(cmd, args) == -1) // SEARCHING FILENAME IN ENVIRONMENT VARIABLE PATH
				for (cmd_ptr = cmd_env, pths = getenv("PATH"); *pths != '\0'; pths++, cmd_ptr++)
					if (*pths != ':')
						*cmd_ptr = *pths; // COPY PATH UNTIL ':' FOUND
					else
					{
						*cmd_ptr++ = '/'; // ADD '/'
						*cmd_ptr = '\0';  // AND NULL
						strcat(cmd_env, cmd); // BUILDING NEW COMMAND ["CMD_ENV"] = ["PATH"] + ["CMD"]
						if (execv(cmd_env, args) == -1) // TRY NEXT PATH IF RETURN VAL. = -1
							cmd_ptr = cmd_env;			// -> RESET POINTER
					}

		if (cpid == 0) // IF NOT EXECUTED SUCCESSFULLY -> EXIT CHILD-PROCESS -> NEXT INPUT
			exit(1);

		wait(&stat);   // WAIT FOR CHILD PROCESS TO END
		free(args);    // CLEAR ALLOCATED MEMORY
	}
}