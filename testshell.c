/**
 * Rob Casale, Mariam Tsilosani, Richard McEwan, Victor Kaiser-Pendergast,
 * and Lucas Rivera
 * CS 416
 * Section 4
 * Assignment 5
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct builtins {
    char *name; /* name of function */
    int (*f)(); /* function to execute for the built-in command */
};

/**
 * cd [dirname]
 *   The cd command changes the current working directory to that
 *   specified by the first and only argument. An error is printed
 *   if more than one arguments are present or if changing the
 *   directory fails. Otherwise, the full pathname of the new directory
 *   is printed. See the chdir system call man page and the getcwd
 *   library function. If there is no argument given to the cd command
 *   then change to the user's home directory. You can get this by
 *   reading the envrionment variable HOME via getenv("HOME").
 */
int localcd(int argc, char **argv) {
	if(argc > 1){
		printf("Error: max of one argument");
	}
	else if(argc == 0)
		chdir(getenv("HOME"));
	else{
		char my_cwd[1024];
		getcwd(my_cwd, 1024);
		char* input = strcat(my_cwd, argv[0]);
		int retVal = chdir(input);
		if(retVal== -1)
			printf("Error: not able to change to directory specified.");
		else{
			getcwd(my_cwd, 1024);
			printf("%s",my_cwd);
		}
	}
    return 0;
}

/**
 * exit [value]
 *   The exit command causes the shell process to exit by calling
 *   the exit library function (which, in turn, calls the _exit system call).
 *   If no parameter is provided, then exit returns with an exit code of 0.
 *   Otherwise, exit returns with the error code provided by value. Note that
 *   the exit code will be truncated to eight bits (0..255). No need to print
 *   an error here since chances are that we really want to exit the process,
 *   even if the syntax is wrong.
 */
void localexit(int argc, char **argv) {
	if(argc<2){
		exit(0);
	}
	else{
		exit(atoi(argv[1]));	
	}
}

/**
 * Parses the command that was readin from the terminal
 */
void parseCommand(char *line) {

}

/**
 * Starts the shell that we created and will return the
 * exit status of the process.
 * 
 * Get a command line from the user. You may assume that
 * the entire command is on one line. This line may be a
 * pipeline of one or more commands. 
 * 
 * Break the command and arguments into an argument list
 * (an array of char *). You may assume that no command
 * will have more than 50 arguments.
 *
 * Built-in commands must be implemented as a table of
 * pointers to functions.
 *
 * Each built-in command accepts the same parameters that
 * any external program will get: (int argc, char **argv).
 *
 * Neither of the built-in commands have to function in a
 * pipeline of commands.
 */
int startShell() {
    char *line = NULL;
    size_t size = 0;
    ssize_t read;
    printf("$ ");
    while((read = getline(&line, &size, stdin)) != -1) {
        parseCommand(line);
        if (isatty(0)) {
            fputs(line, stderr);
            printf("the standard input is from a terminal\n");
        } 
        else {
            printf("the standard input is NOT from a terminal\n");
        }
        if (isatty(1)) {
            printf("the standard output is to a terminal\n");
        }
        else {
            printf("the standard input is NOT to a terminal\n");
        }
        printf("$ ");
    }
    printf("\n");
    return 0;
}

/**
 * Your assignment will be a simple shell that will run one command or a
 * pipeline of commands. When each process terminates, the shell will print
 * the exit status for the process.
 * 
 * Your goal in this assignment is to become familiar with the basic set of
 * system calls that let you create processes, establish pipes between them,
 * and detect when a child process has died. This assignment will use the
 * fork, execve, wait, pipe, dup2, chdir, and exit system calls.
 * 
 * You do not have to implement multi-line commands, environment variables,
 * or I/O redirection. 
 */
int main(int argc, char** argv) {
    if(argc > 1) {
        printf("Error: no arguments are needed\n");
        return 1;
    }
    else {
        startShell();
	return 0;
    }
}
