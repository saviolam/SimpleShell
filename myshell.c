// CMPT 300 Spring 2016 - Assignment 1
// Savio Yin Kit Lam (301179247)
// Due Date: 2nd February, 2016
// A SIMPLE SHELL

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/wait.h>
#include <unistd.h>

// Built-in exit function to terminate the shell
int SHELL_exit(char **args)
{
	return 0;
}

// Launch a process with the given command arguments
int SHELL_launch(char **args)
{
	pid_t pid;
	pid_t wait_pid;
	int stat_val;

	pid = fork();	// Forks the process and saves the return process ID

	// If exec command returns -1, print out the error
	if (pid < 0) {
		perror("SHELL ERROR (FORKING FAIL)");
	}
	// Run the command given by the user in the child process
	else if (pid == 0) {
		if (execvp(args[0], args) == -1) {
			perror("SHELL ERROR");
		}
		exit(EXIT_FAILURE);
	}
	// Fork executed successfully and the parent process will land here
	else {
		do {
			wait_pid = waitpid(pid, &stat_val, WUNTRACED);
		} while (!WIFEXITED(stat_val) && !WIFSIGNALED(stat_val));
	}

	return 1;
}

int SHELL_execute(char **args)
{

	// An empty command was entered
	if (args[0] == NULL) {
		return 1;
	}

	// Checks if the user inputted command is "exit"
	if (strcmp(args[0], "exit") == 0) {
		return SHELL_exit(args);
	}

	return SHELL_launch(args);
}

// Scans user command line by the getline() function
char *SHELL_scan_cmd(void)
{
	char *cmd = NULL;
	ssize_t bufferSize = 0; // Have getline allocate a buffer for us
	getline(&cmd, &bufferSize, stdin);
	return cmd;
}

// Split up and tokenize user inputted command line into arguments
char **SHELL_tokenize_cmd(char *cmd)
{
	int bufferSize = 64;
	int position = 0;
	char **tokens = malloc(bufferSize * sizeof(char*));
	char *token;

	if (!tokens) {
		fprintf(stderr, "SHELL: allocation error\n");
		exit(EXIT_FAILURE);
	}

	token = strtok(cmd, " \t\r\n\a");
	while (token != NULL) {
		tokens[position] = token;
		position++;

		if (position >= bufferSize) {
			bufferSize ++;
			tokens = realloc(tokens, bufferSize * sizeof(char*));
			if (!tokens) {
				fprintf(stderr, "SHELL: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}

		token = strtok(NULL, " \t\r\n\a");
	}
	tokens[position] = NULL;
	return tokens;
}

// Prints a prompt, scan, tokenize, and execute user's command line
void SHELL_main(void)
{
	char *cmd;
	char **args;
	int stat_val = 1;

	while (stat_val)
	{
		char current[100];
		time_t now = time (0);
		strftime (current, 100, "%Y-%m-%d %H:%M:%S", localtime (&now));
		printf("%s# ", current);
		cmd = SHELL_scan_cmd();
		args = SHELL_tokenize_cmd(cmd);
		stat_val = SHELL_execute(args);

		free(cmd);
		free(args);
	};
}

int main()
{
	SHELL_main();
	return EXIT_SUCCESS;
}